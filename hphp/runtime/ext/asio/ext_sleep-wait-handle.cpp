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

#include "hphp/runtime/ext/asio/ext_sleep-wait-handle.h"

#include "hphp/runtime/ext/asio/ext_asio.h"
#include "hphp/runtime/ext/asio/asio-blockable.h"
#include "hphp/runtime/ext/asio/asio-context.h"
#include "hphp/runtime/ext/asio/asio-session.h"
#include "hphp/system/systemlib.h"
#include "hphp/util/timer.h"

namespace HPHP {
///////////////////////////////////////////////////////////////////////////////

namespace {
  StaticString s_sleep("<sleep>");
}

void HHVM_STATIC_METHOD(SleepWaitHandle, setOnCreateCallback,
                        const Variant& callback) {
  AsioSession::Get()->setOnSleepCreate(callback);
}

void HHVM_STATIC_METHOD(SleepWaitHandle, setOnSuccessCallback,
                        const Variant& callback) {
  AsioSession::Get()->setOnSleepSuccess(callback);
}

Object HHVM_STATIC_METHOD(SleepWaitHandle, create, int64_t usecs) {
  if (UNLIKELY(usecs < 0)) {
    SystemLib::throwInvalidArgumentExceptionObject(
        "Expected usecs to be a non-negative integer");
  }

  auto wh = req::make<c_SleepWaitHandle>();
  wh->initialize(usecs);
  return Object(std::move(wh));
}

void c_SleepWaitHandle::initialize(int64_t usecs) {
  auto const session = AsioSession::Get();
  setState(STATE_WAITING);
  setContextStateIndex(session->getCurrentContextStateIndex());
  m_waketime =
    AsioSession::TimePoint::clock::now() +
    std::chrono::microseconds(usecs);

  incRefCount();

  session->enqueueSleepEvent(this);

  if (isInContext()) {
    registerToContext();
  }

  if (UNLIKELY(session->hasOnSleepCreate())) {
    session->onSleepCreate(this);
  }
}

bool c_SleepWaitHandle::cancel(const Object& exception) {
  if (getState() != STATE_WAITING) {
    return false;               // already finished
  }

  if (isInContext()) {
    unregisterFromContext();
  }

  auto parentChain = getParentChain();
  setState(STATE_FAILED);
  tvWriteObject(exception.get(), &m_resultOrException);
  parentChain.unblock();

  // this is technically a lie, since sleep failed
  auto session = AsioSession::Get();
  if (UNLIKELY(session->hasOnSleepSuccess())) {
    session->onSleepSuccess(
      this,
      std::chrono::duration_cast<std::chrono::nanoseconds>(
        m_waketime.time_since_epoch()
      ).count()
    );
  }

  return true;
}

bool c_SleepWaitHandle::process() {
  if (getState() == STATE_FAILED) {
    // sleep handle was cancelled, everything is taken care of
    return false;
  }

  assertx(getState() == STATE_WAITING);

  if (isInContext()) {
    unregisterFromContext();
  }

  auto parentChain = getParentChain();
  setState(STATE_SUCCEEDED);
  tvWriteNull(m_resultOrException);
  parentChain.unblock();

  auto session = AsioSession::Get();
  if (UNLIKELY(session->hasOnSleepSuccess())) {
    session->onSleepSuccess(
      this,
      std::chrono::duration_cast<std::chrono::nanoseconds>(
        m_waketime.time_since_epoch()
      ).count()
    );
  }

  return true;
}

String c_SleepWaitHandle::getName() {
  return s_sleep;
}

void c_SleepWaitHandle::exitContext(ContextIndex contextIdx) {
  assertx(AsioSession::Get()->getContext(contextIdx));
  assertx(getState() == STATE_WAITING);
  assertx(getContextIndex() == contextIdx);

  // Move us to the parent context.
  setContextStateIndex(contextIdx.parent().toRegular());

  // Re-register if still in a context.
  if (isInContext()) {
    registerToContext();
  }

  // Recursively move all wait handles blocked by us.
  getParentChain().exitContext(contextIdx);
}

void c_SleepWaitHandle::registerToContext() {
  AsioContext* ctx = getContext();
  m_ctxVecIndex = ctx->registerTo(ctx->getSleepEvents(), this);
}

void c_SleepWaitHandle::unregisterFromContext() {
  AsioContext* ctx = getContext();
  ctx->unregisterFrom(ctx->getSleepEvents(), m_ctxVecIndex);
}

///////////////////////////////////////////////////////////////////////////////

void AsioExtension::registerNativeSleepWaitHandle() {
#define SWH_SME(meth) \
  HHVM_STATIC_MALIAS(HH\\SleepWaitHandle, meth, SleepWaitHandle, meth)
  SWH_SME(create);
  SWH_SME(setOnCreateCallback);
  SWH_SME(setOnSuccessCallback);
#undef SWH_SWE

  Native::registerClassExtraDataHandler(
    c_SleepWaitHandle::className(), finish_class<c_SleepWaitHandle>);
}

///////////////////////////////////////////////////////////////////////////////
}
