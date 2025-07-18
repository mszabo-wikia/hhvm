/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/interactions/src/shared.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */

#include "thrift/compiler/test/fixtures/interactions/gen-cpp2/InteractLocallyAsyncClient.h"

#include <thrift/lib/cpp2/gen/client_cpp.h>

namespace thrift::shared_interactions {
} // namespace thrift::shared_interactions


apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::createSharedInteraction() {
  return SharedInteraction(channel_, "SharedInteraction", interceptors_);
}


namespace thrift::shared_interactions {
typedef apache::thrift::ThriftPresult<false> InteractLocally_SharedInteraction_init_pargs;
typedef apache::thrift::ThriftPresult<true, apache::thrift::FieldData<0, ::apache::thrift::type_class::integral, ::std::int32_t*>> InteractLocally_SharedInteraction_init_presult;
typedef apache::thrift::ThriftPresult<false> InteractLocally_SharedInteraction_do_something_pargs;
typedef apache::thrift::ThriftPresult<true, apache::thrift::FieldData<0, ::apache::thrift::type_class::structure, ::thrift::shared_interactions::DoSomethingResult*>> InteractLocally_SharedInteraction_do_something_presult;
typedef apache::thrift::ThriftPresult<false> InteractLocally_SharedInteraction_tear_down_pargs;
typedef apache::thrift::ThriftPresult<true> InteractLocally_SharedInteraction_tear_down_presult;
} // namespace thrift::shared_interactions
template <typename RpcOptions>
void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_send_init(apache::thrift::SerializedRequest&& request, RpcOptions&& rpcOptions, std::shared_ptr<apache::thrift::transport::THeader> header, apache::thrift::RequestClientCallback::Ptr callback, std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata) {
  apache::thrift::RpcOptions rpcOpts(std::forward<RpcOptions>(rpcOptions));
  setInteraction(rpcOpts);

  static ::apache::thrift::MethodMetadata::Data* methodMetadata =
        new ::apache::thrift::MethodMetadata::Data(
                "SharedInteraction.init",
                ::apache::thrift::FunctionQualifier::Unspecified,
                "InteractLocally",
                ::apache::thrift::InteractionMethodPosition::Member,
                "SharedInteraction");
  apache::thrift::clientSendT<apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE>(std::move(request), std::move(rpcOpts), std::move(callback), std::move(header), channel_.get(), ::apache::thrift::MethodMetadata::from_static(methodMetadata), std::move(interceptorFrameworkMetadata));
}

template <typename RpcOptions>
void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_send_do_something(apache::thrift::SerializedRequest&& request, RpcOptions&& rpcOptions, std::shared_ptr<apache::thrift::transport::THeader> header, apache::thrift::RequestClientCallback::Ptr callback, std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata) {
  apache::thrift::RpcOptions rpcOpts(std::forward<RpcOptions>(rpcOptions));
  setInteraction(rpcOpts);

  static ::apache::thrift::MethodMetadata::Data* methodMetadata =
        new ::apache::thrift::MethodMetadata::Data(
                "SharedInteraction.do_something",
                ::apache::thrift::FunctionQualifier::Unspecified,
                "InteractLocally",
                ::apache::thrift::InteractionMethodPosition::Member,
                "SharedInteraction");
  apache::thrift::clientSendT<apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE>(std::move(request), std::move(rpcOpts), std::move(callback), std::move(header), channel_.get(), ::apache::thrift::MethodMetadata::from_static(methodMetadata), std::move(interceptorFrameworkMetadata));
}

template <typename RpcOptions>
void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_send_tear_down(apache::thrift::SerializedRequest&& request, RpcOptions&& rpcOptions, std::shared_ptr<apache::thrift::transport::THeader> header, apache::thrift::RequestClientCallback::Ptr callback, std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata) {
  apache::thrift::RpcOptions rpcOpts(std::forward<RpcOptions>(rpcOptions));
  setInteraction(rpcOpts);

  static ::apache::thrift::MethodMetadata::Data* methodMetadata =
        new ::apache::thrift::MethodMetadata::Data(
                "SharedInteraction.tear_down",
                ::apache::thrift::FunctionQualifier::Unspecified,
                "InteractLocally",
                ::apache::thrift::InteractionMethodPosition::Member,
                "SharedInteraction");
  apache::thrift::clientSendT<apache::thrift::RpcKind::SINGLE_REQUEST_SINGLE_RESPONSE>(std::move(request), std::move(rpcOpts), std::move(callback), std::move(header), channel_.get(), ::apache::thrift::MethodMetadata::from_static(methodMetadata), std::move(interceptorFrameworkMetadata));
}



void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::init(apache::thrift::RpcOptions& rpcOptions, std::unique_ptr<apache::thrift::RequestCallback> callback) {
  auto [ctx, header] = initCtx(&rpcOptions);
  if (ctx != nullptr) {
    auto argsAsRefs = std::tie();
    ctx->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), header.get(), rpcOptions).throwUnlessValue();
  }
  auto [wrappedCallback, contextStack] = apache::thrift::GeneratedAsyncClient::template prepareRequestClientCallback<false /* kIsOneWay */>(std::move(callback), std::move(ctx));
  fbthrift_serialize_and_send_init(rpcOptions, std::move(header), contextStack, std::move(wrappedCallback));
}

apache::thrift::SerializedRequest apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_serialize_init(const RpcOptions& rpcOptions, apache::thrift::transport::THeader& header, apache::thrift::ContextStack* contextStack) {
  return apache::thrift::detail::ac::withProtocolWriter(apache::thrift::GeneratedAsyncClient::getChannel()->getProtocolId(), [&](auto&& prot) {
    using ProtocolWriter = std::decay_t<decltype(prot)>;
    ::thrift::shared_interactions::InteractLocally_SharedInteraction_init_pargs args;
    const auto sizer = [&](ProtocolWriter* p) { return args.serializedSizeZC(p); };
    const auto writer = [&](ProtocolWriter* p) { args.write(p); };
    return apache::thrift::preprocessSendT<ProtocolWriter>(
        &prot,
        rpcOptions,
        contextStack,
        header,
        "SharedInteraction.init",
        writer,
        sizer,
        channel_->getChecksumSamplingRate());
  });
}

void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_serialize_and_send_init(apache::thrift::RpcOptions& rpcOptions, std::shared_ptr<apache::thrift::transport::THeader> header, apache::thrift::ContextStack* contextStack, apache::thrift::RequestClientCallback::Ptr callback, bool stealRpcOptions) {
  apache::thrift::SerializedRequest request = fbthrift_serialize_init(rpcOptions, *header, contextStack);
  std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata = nullptr;
  if (contextStack != nullptr) {
    interceptorFrameworkMetadata = detail::ContextStackUnsafeAPI(*contextStack).getInterceptorFrameworkMetadata(rpcOptions);
  }
  if (stealRpcOptions) {
    fbthrift_send_init(std::move(request), std::move(rpcOptions), std::move(header), std::move(callback), std::move(interceptorFrameworkMetadata));
  } else {
    fbthrift_send_init(std::move(request), rpcOptions, std::move(header), std::move(callback), std::move(interceptorFrameworkMetadata));
  }
}

std::pair<::apache::thrift::ContextStack::UniquePtr, std::shared_ptr<::apache::thrift::transport::THeader>> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::initCtx(apache::thrift::RpcOptions* rpcOptions) {
  auto header = std::make_shared<apache::thrift::transport::THeader>(
      apache::thrift::transport::THeader::ALLOW_BIG_FRAMES);
  header->setProtocolId(channel_->getProtocolId());
  if (rpcOptions) {
    header->setHeaders(rpcOptions->releaseWriteHeaders());
  }

  auto ctx = apache::thrift::ContextStack::createWithClientContext(
      handlers_,
      interceptors_,
      getServiceName(),
      "InteractLocally.SharedInteraction.init",
      *header);

  return {std::move(ctx), std::move(header)};
}
::std::int32_t apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::sync_init() {
  ::apache::thrift::RpcOptions rpcOptions;
  return sync_init(rpcOptions);
}

::std::int32_t apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::sync_init(apache::thrift::RpcOptions& rpcOptions) {
  apache::thrift::ClientReceiveState returnState;
  apache::thrift::ClientSyncCallback<false> callback(&returnState);
  auto protocolId = apache::thrift::GeneratedAsyncClient::getChannel()->getProtocolId();
  auto evb = apache::thrift::GeneratedAsyncClient::getChannel()->getEventBase();
  auto ctxAndHeader = initCtx(&rpcOptions);
  auto wrappedCallback = apache::thrift::RequestClientCallback::Ptr(&callback);
  auto* contextStack  = ctxAndHeader.first.get();
  if (contextStack != nullptr) {
    auto argsAsRefs = std::tie();
    contextStack->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), ctxAndHeader.second.get(), rpcOptions).throwUnlessValue();
  }
  callback.waitUntilDone(
    evb,
    [&] {
      fbthrift_serialize_and_send_init(rpcOptions, ctxAndHeader.second, ctxAndHeader.first.get(), std::move(wrappedCallback));
    });
  if (contextStack != nullptr) {
    contextStack->processClientInterceptorsOnResponse(returnState.header()).throwUnlessValue();
  }
  if (returnState.isException()) {
    returnState.exception().throw_exception();
  }
  returnState.resetProtocolId(protocolId);
  returnState.resetCtx(std::move(ctxAndHeader.first));
  SCOPE_EXIT {
    if (returnState.header() && !returnState.header()->getHeaders().empty()) {
      rpcOptions.setReadHeaders(returnState.header()->releaseHeaders());
    }
  };
  return folly::fibers::runInMainContext([&] {
      return recv_init(returnState);
  });
}


template <typename CallbackType>
folly::SemiFuture<::std::int32_t> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_semifuture_init(apache::thrift::RpcOptions& rpcOptions) {
  using CallbackHelper = apache::thrift::detail::FutureCallbackHelper<::std::int32_t>;
  folly::Promise<CallbackHelper::PromiseResult> promise;
  auto semifuture = promise.getSemiFuture();
  auto ctxAndHeader = initCtx(&rpcOptions);
  auto wrappedCallbackAndContextStack = apache::thrift::GeneratedAsyncClient::template prepareRequestClientCallback<false /* kIsOneWay */>(
    std::make_unique<CallbackType>(std::move(promise), recv_wrapped_init, channel_),
    std::move(ctxAndHeader.first));
  auto header = std::move(ctxAndHeader.second);
  auto* contextStack = wrappedCallbackAndContextStack.second;
  auto wrappedCallback = std::move(wrappedCallbackAndContextStack.first);
  std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata = nullptr;
  if (contextStack != nullptr) {
    auto argsAsRefs = std::tie();
    if (auto exTry = contextStack->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), header.get(), rpcOptions);
        exTry.hasException()) {
      return folly::makeSemiFuture<::std::int32_t>(std::move(exTry).exception());
    }
    interceptorFrameworkMetadata = detail::ContextStackUnsafeAPI(*contextStack).getInterceptorFrameworkMetadata(rpcOptions);
  }
  apache::thrift::SerializedRequest request = fbthrift_serialize_init(rpcOptions, *header, contextStack);
  fbthrift_send_init(std::move(request), rpcOptions, std::move(header), std::move(wrappedCallback), std::move(interceptorFrameworkMetadata));
  return std::move(semifuture).deferValue(CallbackHelper::processClientInterceptorsAndExtractResult);
}

folly::SemiFuture<::std::int32_t> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::semifuture_init() {
  ::apache::thrift::RpcOptions rpcOptions;
  return semifuture_init(rpcOptions);
}

folly::SemiFuture<::std::int32_t> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::semifuture_init(apache::thrift::RpcOptions& rpcOptions) {
  using CallbackType = apache::thrift::SemiFutureCallback<::std::int32_t>;
  return fbthrift_semifuture_init<CallbackType>(rpcOptions);
}


#if FOLLY_HAS_COROUTINES
#endif // FOLLY_HAS_COROUTINES
folly::exception_wrapper apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::recv_wrapped_init(::std::int32_t& _return, ::apache::thrift::ClientReceiveState& state) {
  if (state.isException()) {
    return std::move(state.exception());
  }
  if (!state.hasResponseBuffer()) {
    return folly::make_exception_wrapper<apache::thrift::TApplicationException>("recv_ called without result");
  }

  using result = ::thrift::shared_interactions::InteractLocally_SharedInteraction_init_presult;
  switch (state.protocolId()) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      apache::thrift::BinaryProtocolReader reader;
      return apache::thrift::detail::ac::recv_wrapped<result>(
          &reader, state, _return);
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      apache::thrift::CompactProtocolReader reader;
      return apache::thrift::detail::ac::recv_wrapped<result>(
          &reader, state, _return);
    }
    default:
    {
    }
  }
  return folly::make_exception_wrapper<apache::thrift::TApplicationException>("Could not find Protocol");
}
::std::int32_t apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::recv_init(::apache::thrift::ClientReceiveState& state) {
  ::std::int32_t _return;
  auto ew = recv_wrapped_init(_return, state);
  if (ew) {
    ew.throw_exception();
  }
  return _return;
}

void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::do_something(apache::thrift::RpcOptions& rpcOptions, std::unique_ptr<apache::thrift::RequestCallback> callback) {
  auto [ctx, header] = do_somethingCtx(&rpcOptions);
  if (ctx != nullptr) {
    auto argsAsRefs = std::tie();
    ctx->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), header.get(), rpcOptions).throwUnlessValue();
  }
  auto [wrappedCallback, contextStack] = apache::thrift::GeneratedAsyncClient::template prepareRequestClientCallback<false /* kIsOneWay */>(std::move(callback), std::move(ctx));
  fbthrift_serialize_and_send_do_something(rpcOptions, std::move(header), contextStack, std::move(wrappedCallback));
}

apache::thrift::SerializedRequest apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_serialize_do_something(const RpcOptions& rpcOptions, apache::thrift::transport::THeader& header, apache::thrift::ContextStack* contextStack) {
  return apache::thrift::detail::ac::withProtocolWriter(apache::thrift::GeneratedAsyncClient::getChannel()->getProtocolId(), [&](auto&& prot) {
    using ProtocolWriter = std::decay_t<decltype(prot)>;
    ::thrift::shared_interactions::InteractLocally_SharedInteraction_do_something_pargs args;
    const auto sizer = [&](ProtocolWriter* p) { return args.serializedSizeZC(p); };
    const auto writer = [&](ProtocolWriter* p) { args.write(p); };
    return apache::thrift::preprocessSendT<ProtocolWriter>(
        &prot,
        rpcOptions,
        contextStack,
        header,
        "SharedInteraction.do_something",
        writer,
        sizer,
        channel_->getChecksumSamplingRate());
  });
}

void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_serialize_and_send_do_something(apache::thrift::RpcOptions& rpcOptions, std::shared_ptr<apache::thrift::transport::THeader> header, apache::thrift::ContextStack* contextStack, apache::thrift::RequestClientCallback::Ptr callback, bool stealRpcOptions) {
  apache::thrift::SerializedRequest request = fbthrift_serialize_do_something(rpcOptions, *header, contextStack);
  std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata = nullptr;
  if (contextStack != nullptr) {
    interceptorFrameworkMetadata = detail::ContextStackUnsafeAPI(*contextStack).getInterceptorFrameworkMetadata(rpcOptions);
  }
  if (stealRpcOptions) {
    fbthrift_send_do_something(std::move(request), std::move(rpcOptions), std::move(header), std::move(callback), std::move(interceptorFrameworkMetadata));
  } else {
    fbthrift_send_do_something(std::move(request), rpcOptions, std::move(header), std::move(callback), std::move(interceptorFrameworkMetadata));
  }
}

std::pair<::apache::thrift::ContextStack::UniquePtr, std::shared_ptr<::apache::thrift::transport::THeader>> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::do_somethingCtx(apache::thrift::RpcOptions* rpcOptions) {
  auto header = std::make_shared<apache::thrift::transport::THeader>(
      apache::thrift::transport::THeader::ALLOW_BIG_FRAMES);
  header->setProtocolId(channel_->getProtocolId());
  if (rpcOptions) {
    header->setHeaders(rpcOptions->releaseWriteHeaders());
  }

  auto ctx = apache::thrift::ContextStack::createWithClientContext(
      handlers_,
      interceptors_,
      getServiceName(),
      "InteractLocally.SharedInteraction.do_something",
      *header);

  return {std::move(ctx), std::move(header)};
}
void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::sync_do_something(::thrift::shared_interactions::DoSomethingResult& _return) {
  ::apache::thrift::RpcOptions rpcOptions;
  sync_do_something(rpcOptions, _return);
}

void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::sync_do_something(apache::thrift::RpcOptions& rpcOptions, ::thrift::shared_interactions::DoSomethingResult& _return) {
  apache::thrift::ClientReceiveState returnState;
  apache::thrift::ClientSyncCallback<false> callback(&returnState);
  auto protocolId = apache::thrift::GeneratedAsyncClient::getChannel()->getProtocolId();
  auto evb = apache::thrift::GeneratedAsyncClient::getChannel()->getEventBase();
  auto ctxAndHeader = do_somethingCtx(&rpcOptions);
  auto wrappedCallback = apache::thrift::RequestClientCallback::Ptr(&callback);
  auto* contextStack  = ctxAndHeader.first.get();
  if (contextStack != nullptr) {
    auto argsAsRefs = std::tie();
    contextStack->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), ctxAndHeader.second.get(), rpcOptions).throwUnlessValue();
  }
  callback.waitUntilDone(
    evb,
    [&] {
      fbthrift_serialize_and_send_do_something(rpcOptions, ctxAndHeader.second, ctxAndHeader.first.get(), std::move(wrappedCallback));
    });
  if (contextStack != nullptr) {
    contextStack->processClientInterceptorsOnResponse(returnState.header()).throwUnlessValue();
  }
  if (returnState.isException()) {
    returnState.exception().throw_exception();
  }
  returnState.resetProtocolId(protocolId);
  returnState.resetCtx(std::move(ctxAndHeader.first));
  SCOPE_EXIT {
    if (returnState.header() && !returnState.header()->getHeaders().empty()) {
      rpcOptions.setReadHeaders(returnState.header()->releaseHeaders());
    }
  };
  return folly::fibers::runInMainContext([&] {
      recv_do_something(_return, returnState);
  });
}


template <typename CallbackType>
folly::SemiFuture<::thrift::shared_interactions::DoSomethingResult> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_semifuture_do_something(apache::thrift::RpcOptions& rpcOptions) {
  using CallbackHelper = apache::thrift::detail::FutureCallbackHelper<::thrift::shared_interactions::DoSomethingResult>;
  folly::Promise<CallbackHelper::PromiseResult> promise;
  auto semifuture = promise.getSemiFuture();
  auto ctxAndHeader = do_somethingCtx(&rpcOptions);
  auto wrappedCallbackAndContextStack = apache::thrift::GeneratedAsyncClient::template prepareRequestClientCallback<false /* kIsOneWay */>(
    std::make_unique<CallbackType>(std::move(promise), recv_wrapped_do_something, channel_),
    std::move(ctxAndHeader.first));
  auto header = std::move(ctxAndHeader.second);
  auto* contextStack = wrappedCallbackAndContextStack.second;
  auto wrappedCallback = std::move(wrappedCallbackAndContextStack.first);
  std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata = nullptr;
  if (contextStack != nullptr) {
    auto argsAsRefs = std::tie();
    if (auto exTry = contextStack->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), header.get(), rpcOptions);
        exTry.hasException()) {
      return folly::makeSemiFuture<::thrift::shared_interactions::DoSomethingResult>(std::move(exTry).exception());
    }
    interceptorFrameworkMetadata = detail::ContextStackUnsafeAPI(*contextStack).getInterceptorFrameworkMetadata(rpcOptions);
  }
  apache::thrift::SerializedRequest request = fbthrift_serialize_do_something(rpcOptions, *header, contextStack);
  fbthrift_send_do_something(std::move(request), rpcOptions, std::move(header), std::move(wrappedCallback), std::move(interceptorFrameworkMetadata));
  return std::move(semifuture).deferValue(CallbackHelper::processClientInterceptorsAndExtractResult);
}

folly::SemiFuture<::thrift::shared_interactions::DoSomethingResult> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::semifuture_do_something() {
  ::apache::thrift::RpcOptions rpcOptions;
  return semifuture_do_something(rpcOptions);
}

folly::SemiFuture<::thrift::shared_interactions::DoSomethingResult> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::semifuture_do_something(apache::thrift::RpcOptions& rpcOptions) {
  using CallbackType = apache::thrift::SemiFutureCallback<::thrift::shared_interactions::DoSomethingResult>;
  return fbthrift_semifuture_do_something<CallbackType>(rpcOptions);
}


#if FOLLY_HAS_COROUTINES
#endif // FOLLY_HAS_COROUTINES
folly::exception_wrapper apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::recv_wrapped_do_something(::thrift::shared_interactions::DoSomethingResult& _return, ::apache::thrift::ClientReceiveState& state) {
  if (state.isException()) {
    return std::move(state.exception());
  }
  if (!state.hasResponseBuffer()) {
    return folly::make_exception_wrapper<apache::thrift::TApplicationException>("recv_ called without result");
  }

  using result = ::thrift::shared_interactions::InteractLocally_SharedInteraction_do_something_presult;
  switch (state.protocolId()) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      apache::thrift::BinaryProtocolReader reader;
      return apache::thrift::detail::ac::recv_wrapped<result>(
          &reader, state, _return);
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      apache::thrift::CompactProtocolReader reader;
      return apache::thrift::detail::ac::recv_wrapped<result>(
          &reader, state, _return);
    }
    default:
    {
    }
  }
  return folly::make_exception_wrapper<apache::thrift::TApplicationException>("Could not find Protocol");
}
void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::recv_do_something(::thrift::shared_interactions::DoSomethingResult& _return, ::apache::thrift::ClientReceiveState& state) {
  auto ew = recv_wrapped_do_something(_return, state);
  if (ew) {
    ew.throw_exception();
  }
}

void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::tear_down(apache::thrift::RpcOptions& rpcOptions, std::unique_ptr<apache::thrift::RequestCallback> callback) {
  auto [ctx, header] = tear_downCtx(&rpcOptions);
  if (ctx != nullptr) {
    auto argsAsRefs = std::tie();
    ctx->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), header.get(), rpcOptions).throwUnlessValue();
  }
  auto [wrappedCallback, contextStack] = apache::thrift::GeneratedAsyncClient::template prepareRequestClientCallback<false /* kIsOneWay */>(std::move(callback), std::move(ctx));
  fbthrift_serialize_and_send_tear_down(rpcOptions, std::move(header), contextStack, std::move(wrappedCallback));
}

apache::thrift::SerializedRequest apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_serialize_tear_down(const RpcOptions& rpcOptions, apache::thrift::transport::THeader& header, apache::thrift::ContextStack* contextStack) {
  return apache::thrift::detail::ac::withProtocolWriter(apache::thrift::GeneratedAsyncClient::getChannel()->getProtocolId(), [&](auto&& prot) {
    using ProtocolWriter = std::decay_t<decltype(prot)>;
    ::thrift::shared_interactions::InteractLocally_SharedInteraction_tear_down_pargs args;
    const auto sizer = [&](ProtocolWriter* p) { return args.serializedSizeZC(p); };
    const auto writer = [&](ProtocolWriter* p) { args.write(p); };
    return apache::thrift::preprocessSendT<ProtocolWriter>(
        &prot,
        rpcOptions,
        contextStack,
        header,
        "SharedInteraction.tear_down",
        writer,
        sizer,
        channel_->getChecksumSamplingRate());
  });
}

void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_serialize_and_send_tear_down(apache::thrift::RpcOptions& rpcOptions, std::shared_ptr<apache::thrift::transport::THeader> header, apache::thrift::ContextStack* contextStack, apache::thrift::RequestClientCallback::Ptr callback, bool stealRpcOptions) {
  apache::thrift::SerializedRequest request = fbthrift_serialize_tear_down(rpcOptions, *header, contextStack);
  std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata = nullptr;
  if (contextStack != nullptr) {
    interceptorFrameworkMetadata = detail::ContextStackUnsafeAPI(*contextStack).getInterceptorFrameworkMetadata(rpcOptions);
  }
  if (stealRpcOptions) {
    fbthrift_send_tear_down(std::move(request), std::move(rpcOptions), std::move(header), std::move(callback), std::move(interceptorFrameworkMetadata));
  } else {
    fbthrift_send_tear_down(std::move(request), rpcOptions, std::move(header), std::move(callback), std::move(interceptorFrameworkMetadata));
  }
}

std::pair<::apache::thrift::ContextStack::UniquePtr, std::shared_ptr<::apache::thrift::transport::THeader>> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::tear_downCtx(apache::thrift::RpcOptions* rpcOptions) {
  auto header = std::make_shared<apache::thrift::transport::THeader>(
      apache::thrift::transport::THeader::ALLOW_BIG_FRAMES);
  header->setProtocolId(channel_->getProtocolId());
  if (rpcOptions) {
    header->setHeaders(rpcOptions->releaseWriteHeaders());
  }

  auto ctx = apache::thrift::ContextStack::createWithClientContext(
      handlers_,
      interceptors_,
      getServiceName(),
      "InteractLocally.SharedInteraction.tear_down",
      *header);

  return {std::move(ctx), std::move(header)};
}
void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::sync_tear_down() {
  ::apache::thrift::RpcOptions rpcOptions;
  sync_tear_down(rpcOptions);
}

void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::sync_tear_down(apache::thrift::RpcOptions& rpcOptions) {
  apache::thrift::ClientReceiveState returnState;
  apache::thrift::ClientSyncCallback<false> callback(&returnState);
  auto protocolId = apache::thrift::GeneratedAsyncClient::getChannel()->getProtocolId();
  auto evb = apache::thrift::GeneratedAsyncClient::getChannel()->getEventBase();
  auto ctxAndHeader = tear_downCtx(&rpcOptions);
  auto wrappedCallback = apache::thrift::RequestClientCallback::Ptr(&callback);
  auto* contextStack  = ctxAndHeader.first.get();
  if (contextStack != nullptr) {
    auto argsAsRefs = std::tie();
    contextStack->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), ctxAndHeader.second.get(), rpcOptions).throwUnlessValue();
  }
  callback.waitUntilDone(
    evb,
    [&] {
      fbthrift_serialize_and_send_tear_down(rpcOptions, ctxAndHeader.second, ctxAndHeader.first.get(), std::move(wrappedCallback));
    });
  if (contextStack != nullptr) {
    contextStack->processClientInterceptorsOnResponse(returnState.header()).throwUnlessValue();
  }
  if (returnState.isException()) {
    returnState.exception().throw_exception();
  }
  returnState.resetProtocolId(protocolId);
  returnState.resetCtx(std::move(ctxAndHeader.first));
  SCOPE_EXIT {
    if (returnState.header() && !returnState.header()->getHeaders().empty()) {
      rpcOptions.setReadHeaders(returnState.header()->releaseHeaders());
    }
  };
  return folly::fibers::runInMainContext([&] {
      recv_tear_down(returnState);
  });
}


template <typename CallbackType>
folly::SemiFuture<folly::Unit> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::fbthrift_semifuture_tear_down(apache::thrift::RpcOptions& rpcOptions) {
  using CallbackHelper = apache::thrift::detail::FutureCallbackHelper<folly::Unit>;
  folly::Promise<CallbackHelper::PromiseResult> promise;
  auto semifuture = promise.getSemiFuture();
  auto ctxAndHeader = tear_downCtx(&rpcOptions);
  auto wrappedCallbackAndContextStack = apache::thrift::GeneratedAsyncClient::template prepareRequestClientCallback<false /* kIsOneWay */>(
    std::make_unique<CallbackType>(std::move(promise), recv_wrapped_tear_down, channel_),
    std::move(ctxAndHeader.first));
  auto header = std::move(ctxAndHeader.second);
  auto* contextStack = wrappedCallbackAndContextStack.second;
  auto wrappedCallback = std::move(wrappedCallbackAndContextStack.first);
  std::unique_ptr<folly::IOBuf> interceptorFrameworkMetadata = nullptr;
  if (contextStack != nullptr) {
    auto argsAsRefs = std::tie();
    if (auto exTry = contextStack->processClientInterceptorsOnRequest(apache::thrift::ClientInterceptorOnRequestArguments(argsAsRefs), header.get(), rpcOptions);
        exTry.hasException()) {
      return folly::makeSemiFuture<folly::Unit>(std::move(exTry).exception());
    }
    interceptorFrameworkMetadata = detail::ContextStackUnsafeAPI(*contextStack).getInterceptorFrameworkMetadata(rpcOptions);
  }
  apache::thrift::SerializedRequest request = fbthrift_serialize_tear_down(rpcOptions, *header, contextStack);
  fbthrift_send_tear_down(std::move(request), rpcOptions, std::move(header), std::move(wrappedCallback), std::move(interceptorFrameworkMetadata));
  return std::move(semifuture).deferValue(CallbackHelper::processClientInterceptorsAndExtractResult);
}

folly::SemiFuture<folly::Unit> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::semifuture_tear_down() {
  ::apache::thrift::RpcOptions rpcOptions;
  return semifuture_tear_down(rpcOptions);
}

folly::SemiFuture<folly::Unit> apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::semifuture_tear_down(apache::thrift::RpcOptions& rpcOptions) {
  using CallbackType = apache::thrift::SemiFutureCallback<folly::Unit>;
  return fbthrift_semifuture_tear_down<CallbackType>(rpcOptions);
}


#if FOLLY_HAS_COROUTINES
#endif // FOLLY_HAS_COROUTINES
folly::exception_wrapper apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::recv_wrapped_tear_down(::apache::thrift::ClientReceiveState& state) {
  if (state.isException()) {
    return std::move(state.exception());
  }
  if (!state.hasResponseBuffer()) {
    return folly::make_exception_wrapper<apache::thrift::TApplicationException>("recv_ called without result");
  }

  using result = ::thrift::shared_interactions::InteractLocally_SharedInteraction_tear_down_presult;
  switch (state.protocolId()) {
    case apache::thrift::protocol::T_BINARY_PROTOCOL:
    {
      apache::thrift::BinaryProtocolReader reader;
      return apache::thrift::detail::ac::recv_wrapped<result>(
          &reader, state);
    }
    case apache::thrift::protocol::T_COMPACT_PROTOCOL:
    {
      apache::thrift::CompactProtocolReader reader;
      return apache::thrift::detail::ac::recv_wrapped<result>(
          &reader, state);
    }
    default:
    {
    }
  }
  return folly::make_exception_wrapper<apache::thrift::TApplicationException>("Could not find Protocol");
}
void apache::thrift::Client<::thrift::shared_interactions::InteractLocally>::SharedInteraction::recv_tear_down(::apache::thrift::ClientReceiveState& state) {
  auto ew = recv_wrapped_tear_down(state);
  if (ew) {
    ew.throw_exception();
  }
}

