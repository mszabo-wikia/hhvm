/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/doctext/src/module.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */
#include "thrift/compiler/test/fixtures/doctext/gen-cpp2/module_types.tcc"

namespace cpp2 {

template void A::readNoXfer<>(apache::thrift::BinaryProtocolReader*);
template uint32_t A::write<>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t A::serializedSize<>(apache::thrift::BinaryProtocolWriter const*) const;
template uint32_t A::serializedSizeZC<>(apache::thrift::BinaryProtocolWriter const*) const;

template void U::readNoXfer<>(apache::thrift::BinaryProtocolReader*);
template uint32_t U::write<>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t U::serializedSize<>(apache::thrift::BinaryProtocolWriter const*) const;
template uint32_t U::serializedSizeZC<>(apache::thrift::BinaryProtocolWriter const*) const;

template void Bang::readNoXfer<>(apache::thrift::BinaryProtocolReader*);
template uint32_t Bang::write<>(apache::thrift::BinaryProtocolWriter*) const;
template uint32_t Bang::serializedSize<>(apache::thrift::BinaryProtocolWriter const*) const;
template uint32_t Bang::serializedSizeZC<>(apache::thrift::BinaryProtocolWriter const*) const;

} // namespace cpp2
