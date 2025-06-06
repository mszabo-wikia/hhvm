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

// Functions only intended to be called from code generated by the Rust Thrift
// code generator.

use std::ffi::CStr;
use std::fmt;
use std::fmt::Display;
use std::future::Future;

use anyhow::Context;
use anyhow::Result;
use anyhow::bail;
use bytes::Buf;
use thiserror::Error;

use crate::ApplicationException;
use crate::BufExt;
use crate::ContextStack;
use crate::Deserialize;
use crate::MessageType;
use crate::Protocol;
use crate::ProtocolEncodedFinal;
use crate::ProtocolReader;
use crate::ProtocolWriter;
use crate::RequestContext;
use crate::ResultInfo;
use crate::ResultType;
use crate::Serialize;
use crate::SerializedMessage;
use crate::Transport;
use crate::serialize;

// Note: `variants_by_number` must be sorted by the i32 values.
pub fn enum_display(
    variants_by_number: &[(&str, i32)],
    formatter: &mut fmt::Formatter,
    number: i32,
) -> fmt::Result {
    match variants_by_number.binary_search_by_key(&number, |entry| entry.1) {
        Ok(i) => variants_by_number[i].0.fmt(formatter),
        Err(_) => number.fmt(formatter),
    }
}

// Note: `variants_by_name` must be sorted by the string values.
pub fn enum_from_str(
    variants_by_name: &[(&str, i32)],
    value: &str,
    type_name: &'static str,
) -> Result<i32> {
    match variants_by_name.binary_search_by_key(&value, |entry| entry.0) {
        Ok(i) => Ok(variants_by_name[i].1),
        Err(_) => bail!("Unable to parse {} as {}", value, type_name),
    }
}

pub fn buf_len<B: Buf>(b: &B) -> anyhow::Result<u32> {
    let length: usize = b.remaining();
    let length: u32 = length.try_into().with_context(|| {
        format!("Unable to report a buffer length of {length} bytes as a `u32`")
    })?;
    Ok(length)
}

pub trait GetTransport<T: Transport> {
    fn transport(&self) -> &T;
}

pub trait SerializeExn {
    type Success;

    fn write_result<P>(res: Result<&Self::Success, &Self>, p: &mut P, function_name: &'static str)
    where
        P: ProtocolWriter;
}

/// Serialize a result as encoded into a generated *Exn type, wrapped in an envelope.
pub fn serialize_result_envelope<P, CTXT, EXN>(
    function_name: &'static str,
    name_cstr: &<CTXT::ContextStack as ContextStack>::Name,
    seqid: u32,
    rctxt: &CTXT,
    ctx_stack: &mut CTXT::ContextStack,
    res: Result<EXN::Success, EXN>,
) -> anyhow::Result<ProtocolEncodedFinal<P>>
where
    P: Protocol,
    EXN: SerializeExn + ResultInfo,
    CTXT: RequestContext<Name = CStr>,
    <CTXT as RequestContext>::ContextStack: ContextStack<Frame = P::Frame>,
    ProtocolEncodedFinal<P>: Clone + Buf + BufExt,
{
    let res = res.as_ref();
    let res_type = match res {
        Ok(_success) => ResultType::Return,
        Err(exn) => {
            let res_type = exn.result_type();
            assert_ne!(res_type, ResultType::Return);
            assert_eq!(exn.exn_is_declared(), res_type == ResultType::Error);
            ctx_stack.on_error(exn.exn_is_declared(), &exn.exn_value())?;
            rctxt.set_user_exception_header(exn.exn_name(), &exn.exn_value())?;
            res_type
        }
    };

    ctx_stack.pre_write()?;
    let envelope = serialize!(P, |p| {
        p.write_message_begin(function_name, res_type.message_type(), seqid);
        EXN::write_result(res, p, function_name);
        p.write_message_end();
    });

    ctx_stack.on_write_data(SerializedMessage {
        protocol: P::PROTOCOL_ID,
        method_name: name_cstr,
        buffer: envelope.clone(),
    })?;
    let bytes_written = buf_len(&envelope)?;
    ctx_stack.post_write(bytes_written)?;

    Ok(envelope)
}

pub fn serialize_stream_item<P, EXN>(
    res: Result<EXN::Success, EXN>,
    function_name: &'static str,
) -> ProtocolEncodedFinal<P>
where
    P: Protocol,
    EXN: SerializeExn,
{
    let res = res.as_ref();
    serialize!(P, |p| EXN::write_result(res, p, function_name))
}

/// Serialize a request with envelope.
pub fn serialize_request_envelope<P, ARGS>(
    name: &str,
    args: &ARGS,
) -> anyhow::Result<ProtocolEncodedFinal<P>>
where
    P: Protocol,
    ARGS: Serialize<P::Sizer> + Serialize<P::Serializer>,
{
    let envelope = serialize!(P, |p| {
        // Note: we send a 0 message sequence ID from clients because
        // this field should not be used by the server (except for some
        // language implementations).
        p.write_message_begin(name, MessageType::Call, 0);
        args.rs_thrift_write(p);
        p.write_message_end();
    });

    Ok(envelope)
}

pub trait DeserializeExn: Sized {
    type Success;
    type Error;

    fn read_result<P>(p: &mut P) -> Result<Result<Self::Success, Self::Error>>
    where
        P: ProtocolReader;
}

/// Deserialize a client response. This deserializes the envelope then
/// deserializes either a reply or an ApplicationException.
pub fn deserialize_response_envelope<P, EXN>(
    de: &mut P::Deserializer,
) -> anyhow::Result<Result<Result<EXN::Success, EXN::Error>, ApplicationException>>
where
    P: Protocol,
    EXN: DeserializeExn,
{
    let (_, message_type, _) = de.read_message_begin(|_| ())?;

    let res = match message_type {
        MessageType::Reply => Ok(EXN::read_result(de)?),
        MessageType::Exception => Err(ApplicationException::rs_thrift_read(de)?),
        MessageType::Call | MessageType::Oneway | MessageType::InvalidMessageType => {
            bail!("Unwanted message type `{:?}`", message_type)
        }
    };

    de.read_message_end()?;

    Ok(res)
}

#[derive(Debug, Error)]
pub enum SpawnerError {
    #[error("runtime shutting down")]
    RuntimeShuttingDown,
}
/// Abstract spawning some potentially CPU-heavy work onto a CPU thread
pub trait Spawner: 'static {
    fn spawn<F, R>(func: F) -> impl Future<Output = Result<R, SpawnerError>> + Send
    where
        F: FnOnce() -> R + Send + 'static,
        R: Send + 'static;
}

/// No-op implementation of Spawner - just run on current thread
pub struct NoopSpawner;
impl Spawner for NoopSpawner {
    #[inline]
    async fn spawn<F, R>(func: F) -> Result<R, SpawnerError>
    where
        F: FnOnce() -> R + Send + 'static,
        R: Send + 'static,
    {
        Ok(func())
    }
}

pub async fn async_deserialize_response_envelope<P, EXN, S>(
    mut de: P::Deserializer,
) -> anyhow::Result<Result<Result<EXN::Success, EXN::Error>, ApplicationException>>
where
    P: Protocol,
    P::Deserializer: Send,
    EXN: DeserializeExn + Send + 'static,
    EXN::Success: Send + 'static,
    EXN::Error: Send + 'static,
    S: Spawner,
{
    S::spawn(move || deserialize_response_envelope::<P, EXN>(&mut de)).await?
}
