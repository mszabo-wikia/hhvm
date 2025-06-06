/*
   +----------------------------------------------------------------------+
   | HipHop for PHP                                                       |
   +----------------------------------------------------------------------+
   | Copyright (c) 2010-present Facebook, Inc. (http://www.facebook.com)  |
   | Copyright (c) 1997-2010 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
*/

#pragma once

#include "hphp/runtime/ext/extension.h"
#include "hphp/runtime/base/request-info.h"
#include "hphp/runtime/base/type-array.h"
#include "hphp/runtime/base/type-object.h"
#include "hphp/runtime/ext/asio/asio-context.h"
#include "hphp/runtime/ext/asio/asio-external-thread-event-queue.h"
#include "hphp/util/rds-local.h"

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

struct ActRec;
struct c_Awaitable;
struct c_AwaitAllWaitHandle;
struct c_ConcurrentWaitHandle;
struct c_ConditionWaitHandle;
struct c_ResumableWaitHandle;
struct c_PriorityBridgeWaitHandle;

struct AsioSession final {
  static void Init();
  static AsioSession* Get() { return *s_current; }

  // context
  void enterContext(ActRec* savedFP);
  void exitContext();

  bool isInContext() {
    return !m_contexts.empty();
  }

  AsioContext* getContext(ContextIndex ctxIdx) {
    assertx(ctxIdx.value <= m_contexts.size());
    return ctxIdx.value ? m_contexts[ctxIdx.value - 1] : nullptr;
  }

  AsioContext* getCurrentContext() {
    assertx(m_currCtxStateIdx.contextIndex().value == m_contexts.size());
    return m_contexts.back();
  }

  void setCurrentContextStateIndex(ContextStateIndex ctxStateIdx) {
    assertx(ctxStateIdx.contextIndex().value == m_contexts.size());
    m_currCtxStateIdx = ctxStateIdx;
  }

  size_t getCurrentContextDepth() {
    return m_contexts.size();
  }

  ContextStateIndex getCurrentContextStateIndex() {
    assertx(m_currCtxStateIdx.contextIndex().value == m_contexts.size());
    return m_currCtxStateIdx;
  }

  // External thread events.
  AsioExternalThreadEventQueue* getExternalThreadEventQueue() {
    return &m_externalThreadEventQueue;
  }

  // Meager time abstractions.
  using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

  // The latest time we will wait for an I/O operation to complete.  If this
  // time is exceeded, onIOWaitExit will throw after checking surprise.
  static TimePoint getLatestWakeTime() {
    auto now = std::chrono::steady_clock::now();
    auto info = RequestInfo::s_requestInfo.getNoCheck();
    auto& data = info->m_reqInjectionData;
    if (!data.getTimeout()) {
      // Don't wait for over nine thousand hours.
      return now + std::chrono::hours(9000);
    }
    auto remaining = int64_t(data.getRemainingTime());
    return now + std::chrono::seconds(remaining);
  }

  // Sleep event management.
  void enqueueSleepEvent(c_SleepWaitHandle* h);
  bool processSleepEvents();
  // Wakeup time of next sleep wait handle or request timeout time.
  // The returned timestamp may correspond to canceled wait handle.
  TimePoint sleepWakeTime();
  // The next wait handle to wake up. The wait handle may be cancled
  c_SleepWaitHandle* nextSleepEvent();

  // Abrupt interrupt exception.
  ObjectData* getAbruptInterruptException() {
    return m_abruptInterruptException.get();
  }
  bool hasAbruptInterruptException() { return !!m_abruptInterruptException; }
  void initAbruptInterruptException();

  // Awaitable callbacks:
  void setOnIOWaitEnter(const Variant& callback);
  void setOnIOWaitExit(const Variant& callback);
  void setOnJoin(const Variant& callback);
  bool hasOnIOWaitEnter() { return !!m_onIOWaitEnter; }
  bool hasOnIOWaitExit() { return !!m_onIOWaitExit; }
  bool hasOnJoin() { return !!m_onJoin; }
  void onIOWaitEnter();
  void onIOWaitExit(c_WaitableWaitHandle* waitHandle);
  void onJoin(c_WaitableWaitHandle* waitHandle);

  // ResumableWaitHandle callbacks:
  void setOnResumableCreate(const Variant& callback);
  void setOnResumableAwait(const Variant& callback);
  void setOnResumableSuccess(const Variant& callback);
  void setOnResumableFail(const Variant& callback);
  bool hasOnResumableCreate() { return !!m_onResumableCreate; }
  bool hasOnResumableAwait() { return !!m_onResumableAwait; }
  bool hasOnResumableSuccess() { return !!m_onResumableSuccess; }
  bool hasOnResumableFail() { return !!m_onResumableFail; }
  void onResumableCreate(c_ResumableWaitHandle*, c_WaitableWaitHandle* child);
  void onResumableAwait(c_ResumableWaitHandle*, c_WaitableWaitHandle* child);
  void onResumableSuccess(c_ResumableWaitHandle* cont, const Variant& result);
  void onResumableFail(c_ResumableWaitHandle* cont, const Object& exception);
  void updateEventHookState();

  // AwaitAllWaitHandle callbacks:
  void setOnAwaitAllCreate(const Variant& callback);
  bool hasOnAwaitAllCreate() { return !!m_onAwaitAllCreate; }
  void onAwaitAllCreate(c_AwaitAllWaitHandle* wh, Array&& dependencies);

  // ConcurrentWaitHandle callbacks:
  void setOnConcurrentCreate(const Variant& callback);
  bool hasOnConcurrentCreate() { return !!m_onConcurrentCreate; }
  void onConcurrentCreate(c_ConcurrentWaitHandle* wh, Array&& dependencies);

  // ConditionWaitHandle callbacks:
  void setOnConditionCreate(const Variant& callback);
  bool hasOnConditionCreate() { return !!m_onConditionCreate; }
  void onConditionCreate(c_ConditionWaitHandle* wh, c_WaitableWaitHandle*);

  // ExternalThreadEventWaitHandle callbacks:
  void setOnExternalThreadEventCreate(const Variant& callback);
  void setOnExternalThreadEventSuccess(const Variant& callback);
  void setOnExternalThreadEventFail(const Variant& callback);
  bool hasOnExternalThreadEventCreate() { return !!m_onExtThreadEventCreate; }
  bool hasOnExternalThreadEventSuccess() { return !!m_onExtThreadEventSuccess; }
  bool hasOnExternalThreadEventFail() { return !!m_onExtThreadEventFail; }
  void onExternalThreadEventCreate(c_ExternalThreadEventWaitHandle* waitHandle);
  void onExternalThreadEventSuccess(c_ExternalThreadEventWaitHandle* waitHandle,
                                    const Variant& result, int64_t finish_time);
  void onExternalThreadEventFail(c_ExternalThreadEventWaitHandle* waitHandle,
                                 const Object& exception, int64_t finish_time);

  // SleepWaitHandle callbacks:
  void setOnSleepCreate(const Variant& callback);
  void setOnSleepSuccess(const Variant& callback);
  bool hasOnSleepCreate() { return !!m_onSleepCreate; }
  bool hasOnSleepSuccess() { return !!m_onSleepSuccess; }
  void onSleepCreate(c_SleepWaitHandle* waitHandle);
  void onSleepSuccess(c_SleepWaitHandle* waitHandle, int64_t finish_time);

  // PriorityBridgeWaitHandle callbacks:
  void setOnPriorityBridgeCreate(const Variant& callback);
  bool hasOnPriorityBridgeCreate() { return !!m_onPriorityBridgeCreate; }
  void onPriorityBridgeCreate(c_PriorityBridgeWaitHandle* wh,
                              c_WaitableWaitHandle* child);

private:
  AsioSession();
  friend AsioSession* req::make_raw<AsioSession>();

private:
  static RDS_LOCAL_NO_CHECK(AsioSession*, s_current);
  req::vector<AsioContext*> m_contexts;
  req::vector<c_SleepWaitHandle*> m_sleepEvents;
  AsioExternalThreadEventQueue m_externalThreadEventQueue;
  ContextStateIndex m_currCtxStateIdx;

  Object m_abruptInterruptException;
  Object m_onIOWaitEnter;
  Object m_onIOWaitExit;
  Object m_onJoin;
  Object m_onResumableCreate;
  Object m_onResumableAwait;
  Object m_onResumableSuccess;
  Object m_onResumableFail;
  Object m_onAwaitAllCreate;
  Object m_onConcurrentCreate;
  Object m_onConditionCreate;
  Object m_onExtThreadEventCreate;
  Object m_onExtThreadEventSuccess;
  Object m_onExtThreadEventFail;
  Object m_onSleepCreate;
  Object m_onSleepSuccess;
  Object m_onPriorityBridgeCreate;
};

///////////////////////////////////////////////////////////////////////////////
}
