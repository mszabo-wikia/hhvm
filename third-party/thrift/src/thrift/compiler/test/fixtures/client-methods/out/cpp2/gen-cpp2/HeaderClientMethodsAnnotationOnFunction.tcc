/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/client-methods/src/module.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */
#pragma once

#include "thrift/compiler/test/fixtures/client-methods/gen-cpp2/HeaderClientMethodsAnnotationOnFunction.h"

#include <thrift/lib/cpp2/gen/service_tcc.h>

namespace cpp2 {
typedef apache::thrift::ThriftPresult<false, apache::thrift::FieldData<1, ::apache::thrift::type_class::structure, ::cpp2::EchoRequest*>> HeaderClientMethodsAnnotationOnFunction_echo_pargs;
typedef apache::thrift::ThriftPresult<true, apache::thrift::FieldData<0, ::apache::thrift::type_class::structure, ::cpp2::EchoResponse*>> HeaderClientMethodsAnnotationOnFunction_echo_presult;
typedef apache::thrift::ThriftPresult<false, apache::thrift::FieldData<1, ::apache::thrift::type_class::structure, ::cpp2::EchoRequest*>> HeaderClientMethodsAnnotationOnFunction_echo_2_pargs;
typedef apache::thrift::ThriftPresult<true, apache::thrift::FieldData<0, ::apache::thrift::type_class::structure, ::cpp2::EchoResponse*>> HeaderClientMethodsAnnotationOnFunction_echo_2_presult;
template <typename ProtocolIn_, typename ProtocolOut_>
void HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::setUpAndProcess_echo(apache::thrift::ResponseChannelRequest::UniquePtr req, apache::thrift::SerializedCompressedRequest&& serializedRequest, apache::thrift::Cpp2RequestContext* ctx, folly::EventBase* eb, [[maybe_unused]] apache::thrift::concurrency::ThreadManager* tm) {
  if (!setUpRequestProcessing(req, ctx, eb, tm, apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE, iface_)) {
    return;
  }
  auto scope = iface_->getRequestExecutionScope(ctx, apache::thrift::concurrency::NORMAL);
  ctx->setRequestExecutionScope(std::move(scope));
  processInThread(std::move(req), std::move(serializedRequest), ctx, eb, tm, apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE, &HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::executeRequest_echo<ProtocolIn_, ProtocolOut_>, this);
}

template <typename ProtocolIn_, typename ProtocolOut_>
void HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::executeRequest_echo(apache::thrift::ServerRequest&& serverRequest) {
  // make sure getRequestContext is null
  // so async calls don't accidentally use it
  iface_->setRequestContext(nullptr);
  struct ArgsState {
    std::unique_ptr<::cpp2::EchoRequest> uarg_request = std::make_unique<::cpp2::EchoRequest>();
    HeaderClientMethodsAnnotationOnFunction_echo_pargs pargs() {
      HeaderClientMethodsAnnotationOnFunction_echo_pargs args;
      args.get<0>().value = uarg_request.get();
      return args;
    }

    auto asTupleOfRefs() & {
      return std::tie(
        std::as_const(*uarg_request)
      );
    }
  } args;

  auto ctxStack = apache::thrift::ContextStack::create(
    this->getEventHandlersSharedPtr(),
    this->getServiceName(),
    "HeaderClientMethodsAnnotationOnFunction.echo",
    serverRequest.requestContext());
  try {
    auto pargs = args.pargs();
    deserializeRequest<ProtocolIn_>(pargs, "echo", apache::thrift::detail::ServerRequestHelper::compressedRequest(std::move(serverRequest)).uncompress(), ctxStack.get());
  }
  catch (...) {
    folly::exception_wrapper ew(std::current_exception());
    apache::thrift::detail::ap::process_handle_exn_deserialization<ProtocolOut_>(
        ew
        , apache::thrift::detail::ServerRequestHelper::request(std::move(serverRequest))
        , serverRequest.requestContext()
        , apache::thrift::detail::ServerRequestHelper::eventBase(serverRequest)
        , "echo");
    return;
  }
  auto requestPileNotification = apache::thrift::detail::ServerRequestHelper::moveRequestPileNotification(serverRequest);
  auto concurrencyControllerNotification = apache::thrift::detail::ServerRequestHelper::moveConcurrencyControllerNotification(serverRequest);
  apache::thrift::HandlerCallbackBase::MethodNameInfo methodNameInfo{
    /* .serviceName =*/ this->getServiceName(),
    /* .definingServiceName =*/ "HeaderClientMethodsAnnotationOnFunction",
    /* .methodName =*/ "echo",
    /* .qualifiedMethodName =*/ "HeaderClientMethodsAnnotationOnFunction.echo"};
  auto callback = apache::thrift::HandlerCallbackPtr<std::unique_ptr<::cpp2::EchoResponse>>::make(
    apache::thrift::detail::ServerRequestHelper::request(std::move(serverRequest))
    , std::move(ctxStack)
    , std::move(methodNameInfo)
    , return_echo<ProtocolIn_,ProtocolOut_>
    , throw_wrapped_echo<ProtocolIn_, ProtocolOut_>
    , serverRequest.requestContext()->getProtoSeqId()
    , apache::thrift::detail::ServerRequestHelper::eventBase(serverRequest)
    , apache::thrift::detail::ServerRequestHelper::executor(serverRequest)
    , serverRequest.requestContext()
    , requestPileNotification
    , concurrencyControllerNotification, std::move(serverRequest.requestData())
    );
  const auto makeExecuteHandler = [&] {
    return [ifacePtr = iface_](auto&& cb, ArgsState args) mutable {
      (void)args;
      ifacePtr->async_tm_echo(std::move(cb), std::move(args.uarg_request));
    };
  };
#if FOLLY_HAS_COROUTINES
  if (apache::thrift::detail::shouldProcessServiceInterceptorsOnRequest(*callback)) {
    [](auto callback, auto executeHandler, ArgsState args) -> folly::coro::Task<void> {
      auto argRefs = args.asTupleOfRefs();
      co_await apache::thrift::detail::processServiceInterceptorsOnRequest(
          *callback,
          apache::thrift::detail::ServiceInterceptorOnRequestArguments(argRefs));
      executeHandler(std::move(callback), std::move(args));
    }(std::move(callback), makeExecuteHandler(), std::move(args))
              .scheduleOn(apache::thrift::detail::ServerRequestHelper::executor(serverRequest))
              .startInlineUnsafe();
  } else {
    makeExecuteHandler()(std::move(callback), std::move(args));
  }
#else
  makeExecuteHandler()(std::move(callback), std::move(args));
#endif // FOLLY_HAS_COROUTINES
}

template <class ProtocolIn_, class ProtocolOut_>
apache::thrift::SerializedResponse HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::return_echo(apache::thrift::ContextStack* ctx, ::cpp2::EchoResponse const& _return) {
  ProtocolOut_ prot;
  ::cpp2::HeaderClientMethodsAnnotationOnFunction_echo_presult result;
  result.get<0>().value = const_cast<::cpp2::EchoResponse*>(&_return);
  result.setIsSet(0, true);
  return serializeResponse("echo", &prot, ctx, result);
}

template <class ProtocolIn_, class ProtocolOut_>
void HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::throw_wrapped_echo(apache::thrift::ResponseChannelRequest::UniquePtr req,[[maybe_unused]] int32_t protoSeqId,apache::thrift::ContextStack* ctx,folly::exception_wrapper ew,apache::thrift::Cpp2RequestContext* reqCtx) {
  if (!ew) {
    return;
  }
  {
    apache::thrift::detail::ap::process_throw_wrapped_handler_error<ProtocolOut_>(
        ew, std::move(req), reqCtx, ctx, "echo");
    return;
  }
}

template <typename ProtocolIn_, typename ProtocolOut_>
void HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::setUpAndProcess_echo_2(apache::thrift::ResponseChannelRequest::UniquePtr req, apache::thrift::SerializedCompressedRequest&& serializedRequest, apache::thrift::Cpp2RequestContext* ctx, folly::EventBase* eb, [[maybe_unused]] apache::thrift::concurrency::ThreadManager* tm) {
  if (!setUpRequestProcessing(req, ctx, eb, tm, apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE, iface_)) {
    return;
  }
  auto scope = iface_->getRequestExecutionScope(ctx, apache::thrift::concurrency::NORMAL);
  ctx->setRequestExecutionScope(std::move(scope));
  processInThread(std::move(req), std::move(serializedRequest), ctx, eb, tm, apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE, &HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::executeRequest_echo_2<ProtocolIn_, ProtocolOut_>, this);
}

template <typename ProtocolIn_, typename ProtocolOut_>
void HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::executeRequest_echo_2(apache::thrift::ServerRequest&& serverRequest) {
  // make sure getRequestContext is null
  // so async calls don't accidentally use it
  iface_->setRequestContext(nullptr);
  struct ArgsState {
    std::unique_ptr<::cpp2::EchoRequest> uarg_request = std::make_unique<::cpp2::EchoRequest>();
    HeaderClientMethodsAnnotationOnFunction_echo_2_pargs pargs() {
      HeaderClientMethodsAnnotationOnFunction_echo_2_pargs args;
      args.get<0>().value = uarg_request.get();
      return args;
    }

    auto asTupleOfRefs() & {
      return std::tie(
        std::as_const(*uarg_request)
      );
    }
  } args;

  auto ctxStack = apache::thrift::ContextStack::create(
    this->getEventHandlersSharedPtr(),
    this->getServiceName(),
    "HeaderClientMethodsAnnotationOnFunction.echo_2",
    serverRequest.requestContext());
  try {
    auto pargs = args.pargs();
    deserializeRequest<ProtocolIn_>(pargs, "echo_2", apache::thrift::detail::ServerRequestHelper::compressedRequest(std::move(serverRequest)).uncompress(), ctxStack.get());
  }
  catch (...) {
    folly::exception_wrapper ew(std::current_exception());
    apache::thrift::detail::ap::process_handle_exn_deserialization<ProtocolOut_>(
        ew
        , apache::thrift::detail::ServerRequestHelper::request(std::move(serverRequest))
        , serverRequest.requestContext()
        , apache::thrift::detail::ServerRequestHelper::eventBase(serverRequest)
        , "echo_2");
    return;
  }
  auto requestPileNotification = apache::thrift::detail::ServerRequestHelper::moveRequestPileNotification(serverRequest);
  auto concurrencyControllerNotification = apache::thrift::detail::ServerRequestHelper::moveConcurrencyControllerNotification(serverRequest);
  apache::thrift::HandlerCallbackBase::MethodNameInfo methodNameInfo{
    /* .serviceName =*/ this->getServiceName(),
    /* .definingServiceName =*/ "HeaderClientMethodsAnnotationOnFunction",
    /* .methodName =*/ "echo_2",
    /* .qualifiedMethodName =*/ "HeaderClientMethodsAnnotationOnFunction.echo_2"};
  auto callback = apache::thrift::HandlerCallbackPtr<std::unique_ptr<::cpp2::EchoResponse>>::make(
    apache::thrift::detail::ServerRequestHelper::request(std::move(serverRequest))
    , std::move(ctxStack)
    , std::move(methodNameInfo)
    , return_echo_2<ProtocolIn_,ProtocolOut_>
    , throw_wrapped_echo_2<ProtocolIn_, ProtocolOut_>
    , serverRequest.requestContext()->getProtoSeqId()
    , apache::thrift::detail::ServerRequestHelper::eventBase(serverRequest)
    , apache::thrift::detail::ServerRequestHelper::executor(serverRequest)
    , serverRequest.requestContext()
    , requestPileNotification
    , concurrencyControllerNotification, std::move(serverRequest.requestData())
    );
  const auto makeExecuteHandler = [&] {
    return [ifacePtr = iface_](auto&& cb, ArgsState args) mutable {
      (void)args;
      ifacePtr->async_tm_echo_2(std::move(cb), std::move(args.uarg_request));
    };
  };
#if FOLLY_HAS_COROUTINES
  if (apache::thrift::detail::shouldProcessServiceInterceptorsOnRequest(*callback)) {
    [](auto callback, auto executeHandler, ArgsState args) -> folly::coro::Task<void> {
      auto argRefs = args.asTupleOfRefs();
      co_await apache::thrift::detail::processServiceInterceptorsOnRequest(
          *callback,
          apache::thrift::detail::ServiceInterceptorOnRequestArguments(argRefs));
      executeHandler(std::move(callback), std::move(args));
    }(std::move(callback), makeExecuteHandler(), std::move(args))
              .scheduleOn(apache::thrift::detail::ServerRequestHelper::executor(serverRequest))
              .startInlineUnsafe();
  } else {
    makeExecuteHandler()(std::move(callback), std::move(args));
  }
#else
  makeExecuteHandler()(std::move(callback), std::move(args));
#endif // FOLLY_HAS_COROUTINES
}

template <class ProtocolIn_, class ProtocolOut_>
apache::thrift::SerializedResponse HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::return_echo_2(apache::thrift::ContextStack* ctx, ::cpp2::EchoResponse const& _return) {
  ProtocolOut_ prot;
  ::cpp2::HeaderClientMethodsAnnotationOnFunction_echo_2_presult result;
  result.get<0>().value = const_cast<::cpp2::EchoResponse*>(&_return);
  result.setIsSet(0, true);
  return serializeResponse("echo_2", &prot, ctx, result);
}

template <class ProtocolIn_, class ProtocolOut_>
void HeaderClientMethodsAnnotationOnFunctionAsyncProcessor::throw_wrapped_echo_2(apache::thrift::ResponseChannelRequest::UniquePtr req,[[maybe_unused]] int32_t protoSeqId,apache::thrift::ContextStack* ctx,folly::exception_wrapper ew,apache::thrift::Cpp2RequestContext* reqCtx) {
  if (!ew) {
    return;
  }
  {
    apache::thrift::detail::ap::process_throw_wrapped_handler_error<ProtocolOut_>(
        ew, std::move(req), reqCtx, ctx, "echo_2");
    return;
  }
}


} // namespace cpp2
