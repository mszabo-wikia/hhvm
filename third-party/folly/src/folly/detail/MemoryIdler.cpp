/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <folly/detail/MemoryIdler.h>

#include <climits>
#include <cstdio>
#include <cstring>
#include <utility>

#include <folly/GLog.h>
#include <folly/Portability.h>
#include <folly/ScopeGuard.h>
#include <folly/concurrency/CacheLocality.h>
#include <folly/memory/MallctlHelper.h>
#include <folly/memory/Malloc.h>
#include <folly/portability/GFlags.h>
#include <folly/portability/PThread.h>
#include <folly/portability/SysMman.h>
#include <folly/portability/Unistd.h>
#include <folly/system/Pid.h>
#include <folly/system/ThreadId.h>

FOLLY_GFLAGS_DEFINE_bool(
    folly_memory_idler_purge_arenas,
    false,
    "if enabled, folly memory-idler purges jemalloc arenas on thread idle");

FOLLY_GFLAGS_DEFINE_bool(
    folly_memory_idler_madvise_stacks,
    true,
    "if enabled, folly memory-idler madvises dontneed stacks on thread idle");

namespace folly {
namespace detail {

AtomicStruct<std::chrono::steady_clock::duration>
    MemoryIdler::defaultIdleTimeout(std::chrono::seconds(5));

bool MemoryIdler::isUnmapUnusedStackAvailable() noexcept {
  // Linux uses an automatic stack expansion mechanism to expand the main thread
  // stack on demand. Before the main thread stack grows to its full extent, the
  // vma corresponding to the main thread stack is not yet fully allocated. It's
  // possible for the kernel to allocate the not-yet-allocated main thread stack
  // vma to ramdon sbrk() or mmap() requests, and for the resulting regions from
  // these requests to be used by other user code. If this happens, the madvise-
  // dontneed here is dangerous - it can zero arbitrary heap buffers! So it must
  // be skipped. In the case where this runs a fork() child in that thread which
  // returned from fork(), the os-thread-id will coincide with the pid, which is
  // a harmless false positive where the madvise-dontneed will be skipped.
  if (kIsLinux && getOSThreadID() == static_cast<uint64_t>(get_cached_pid())) {
    return false;
  }

  return true;
}

void MemoryIdler::flushLocalMallocCaches() {
  if (!usingJEMalloc()) {
    return;
  }
  if (!mallctl || !mallctlnametomib || !mallctlbymib) {
    FB_LOG_EVERY_MS(ERROR, 10000) << "mallctl* weak link failed";
    return;
  }

  // Not using mallctlCall as this will fail if tcache is disabled.
  mallctl("thread.tcache.flush", nullptr, nullptr, nullptr, 0);

  if (FLAGS_folly_memory_idler_purge_arenas) {
    try {
      // By default jemalloc has 4 arenas per cpu, and then assigns each
      // thread to one of those arenas.  This means that in any service
      // that doesn't perform a lot of context switching, the chances that
      // another thread will be using the current thread's arena (and hence
      // doing the appropriate dirty-page purging) are low.  Some good
      // tuned configurations (such as that used by hhvm) use fewer arenas
      // and then pin threads to avoid contended access.  In that case,
      // purging the arenas is counter-productive.  We use the heuristic
      // that if narenas <= 2 * num_cpus then we shouldn't do anything here,
      // which detects when the narenas has been reduced from the default
      unsigned narenas;
      unsigned arenaForCurrent;
      size_t mib[3];
      size_t miblen = 3;

      mallctlRead("opt.narenas", &narenas);
      mallctlRead("thread.arena", &arenaForCurrent);
      if (narenas > 2 * CacheLocality::system().numCpus &&
          mallctlnametomib("arena.0.purge", mib, &miblen) == 0) {
        mib[1] = static_cast<size_t>(arenaForCurrent);
        mallctlbymib(mib, miblen, nullptr, nullptr, nullptr, 0);
      }
    } catch (const std::runtime_error& ex) {
      FB_LOG_EVERY_MS(WARNING, 10000) << ex.what();
    }
  }
}

// Stack madvise isn't Linux or glibc specific, but the system calls
// and arithmetic (and bug compatibility) are not portable.  The set of
// platforms could be increased if it was useful.
#if defined(__GLIBC__) && defined(__linux__) && !FOLLY_MOBILE && \
    (!defined(FOLLY_SANITIZE_ADDRESS) || !FOLLY_SANITIZE_ADDRESS)

static thread_local uintptr_t tls_stackLimit;
static thread_local size_t tls_stackSize;

static size_t pageSize() {
  static const size_t s_pageSize = sysconf(_SC_PAGESIZE);
  return s_pageSize;
}

static void fetchStackLimits() {
  int err;
  pthread_attr_t attr;
  if ((err = pthread_getattr_np(pthread_self(), &attr))) {
    // some restricted environments can't access /proc
    FB_LOG_ONCE(ERROR) << "pthread_getaddr_np failed errno=" << err;
    tls_stackSize = 1;
    return;
  }
  SCOPE_EXIT {
    pthread_attr_destroy(&attr);
  };

  void* addr;
  size_t rawSize;
  if ((err = pthread_attr_getstack(&attr, &addr, &rawSize))) {
    // unexpected, but it is better to continue in prod than do nothing
    FB_LOG_ONCE(ERROR) << "pthread_attr_getstack error " << err;
    assert(false);
    tls_stackSize = 1;
    return;
  }
  if (rawSize >= (1ULL << 32)) {
    // Avoid unmapping huge swaths of memory if there is an insane
    // stack size.  The boundary of sanity is somewhat arbitrary: 4GB.
    //
    // If we went into /proc to find the actual contiguous mapped pages
    // before unmapping we wouldn't care about the stack size at all,
    // but our current strategy is to unmap the entire range that might
    // be used for the stack even if it hasn't been fully faulted-in.
    //
    // Very large stack size is a bug (hence the assert), but we can
    // carry on if we are in prod.
    FB_LOG_ONCE(ERROR)
        << "pthread_attr_getstack returned insane stack size " << rawSize;
    assert(false);
    tls_stackSize = 1;
    return;
  }
  assert(addr != nullptr);
  assert(
      0 < PTHREAD_STACK_MIN &&
      rawSize >= static_cast<size_t>(PTHREAD_STACK_MIN));

  // glibc subtracts guard page from stack size, even though pthread docs
  // seem to imply the opposite
  size_t guardSize;
  if (pthread_attr_getguardsize(&attr, &guardSize) != 0) {
    guardSize = 0;
  }
  assert(rawSize > guardSize);

  // stack goes down, so guard page adds to the base addr
  tls_stackLimit = reinterpret_cast<uintptr_t>(addr) + guardSize;
  tls_stackSize = rawSize - guardSize;

  assert((tls_stackLimit & (pageSize() - 1)) == 0);
}

FOLLY_NOINLINE static uintptr_t getStackPtr() {
  char marker;
  auto rv = reinterpret_cast<uintptr_t>(&marker);
  return rv;
}

void MemoryIdler::unmapUnusedStack(size_t retain) {
  if (!FLAGS_folly_memory_idler_madvise_stacks) {
    return;
  }

  if (!isUnmapUnusedStackAvailable()) {
    return;
  }

  if (tls_stackSize == 0) {
    fetchStackLimits();
  }
  if (tls_stackSize <= std::max(static_cast<size_t>(1), retain)) {
    // covers both missing stack info, and impossibly large retain
    return;
  }

  auto sp = getStackPtr();
  assert(sp >= tls_stackLimit);
  assert(sp - tls_stackLimit < tls_stackSize);

  auto end = (sp - retain) & ~(pageSize() - 1);
  if (end <= tls_stackLimit) {
    // no pages are eligible for unmapping
    return;
  }

  size_t len = end - tls_stackLimit;
  assert((len & (pageSize() - 1)) == 0);
  if (madvise((void*)tls_stackLimit, len, MADV_DONTNEED) != 0) {
    // It is likely that the stack vma hasn't been fully grown.  In this
    // case madvise will apply dontneed to the present vmas, then return
    // errno of ENOMEM.
    // If thread stack pages are backed by locked or huge pages, madvise will
    // fail with EINVAL. (EINVAL may also be returned if the address or length
    // are bad.) Warn in debug mode, since MemoryIdler may not function as
    // expected.
    // We can also get an EAGAIN, theoretically.
    PLOG_IF(WARNING, kIsDebug && errno == EINVAL) << "madvise failed";
    assert(errno == EAGAIN || errno == ENOMEM || errno == EINVAL);
  }
}

#else

void MemoryIdler::unmapUnusedStack(size_t /* retain */) {}

#endif

} // namespace detail
} // namespace folly
