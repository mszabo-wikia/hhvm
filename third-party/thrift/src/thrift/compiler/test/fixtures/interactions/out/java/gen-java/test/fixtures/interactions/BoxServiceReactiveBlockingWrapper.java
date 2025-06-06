/**
 * Autogenerated by Thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

package test.fixtures.interactions;

import java.util.*;

public class BoxServiceReactiveBlockingWrapper 
  implements BoxService {
  private final BoxService.Reactive _delegate;

  public BoxServiceReactiveBlockingWrapper(BoxService.Reactive _delegate) {
    
    this._delegate = _delegate;
  }

  public BoxServiceReactiveBlockingWrapper(org.apache.thrift.ProtocolId _protocolId, reactor.core.publisher.Mono<? extends com.facebook.thrift.client.RpcClient> _rpcClient, Map<String, String> _headers, Map<String, String> _persistentHeaders) {
    this(new BoxServiceReactiveClient(_protocolId, _rpcClient, _headers, _persistentHeaders));
  }

  @java.lang.Override
  public void close() {
    _delegate.dispose();
  }

  @java.lang.Override
  public test.fixtures.interactions.ShouldBeBoxed getABoxSession( final test.fixtures.interactions.ShouldBeBoxed req) throws org.apache.thrift.TException {
      return getABoxSessionWrapper(req, com.facebook.thrift.client.RpcOptions.EMPTY).getData();
  }

  @java.lang.Override
  public test.fixtures.interactions.ShouldBeBoxed getABoxSession(
        final test.fixtures.interactions.ShouldBeBoxed req,
        com.facebook.thrift.client.RpcOptions rpcOptions) throws org.apache.thrift.TException {
      return getABoxSessionWrapper(req,rpcOptions).getData();
  }

  @java.lang.Override
  public com.facebook.thrift.client.ResponseWrapper<test.fixtures.interactions.ShouldBeBoxed> getABoxSessionWrapper(
    final test.fixtures.interactions.ShouldBeBoxed req,
    com.facebook.thrift.client.RpcOptions rpcOptions) throws org.apache.thrift.TException {
      try {
        return _delegate.getABoxSessionWrapper(req, rpcOptions).block();
      } catch (Throwable t) {
        throw com.facebook.thrift.util.ExceptionUtil.wrap(t);
      }
  }

  public BoxedInteraction createBoxedInteraction() {
      throw new UnsupportedOperationException("Interactions are not yet supported on ReactiveBlockingWrapper Interfaces!");
  }
}
