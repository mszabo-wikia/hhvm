/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/basic-annotations/src/module.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */

#include "thrift/compiler/test/fixtures/basic-annotations/gen-cpp2/module_constants.h"

#include <thrift/lib/cpp2/gen/module_constants_cpp.h>


namespace cpp2 {
namespace module_constants {

::cpp2::YourStruct const& myStruct() {
  static folly::Indestructible<::cpp2::YourStruct> const instance{ ::StaticCast::fromThrift(::cpp2::detail::YourStruct(::apache::thrift::detail::make_structured_constant<::cpp2::detail::YourStruct>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::majorVer>(static_cast<::std::int64_t>(42)), ::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::abstract>(apache::thrift::StringTraits<::std::string>::fromStringLiteral("abstract")), ::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::my_enum>( ::cpp2::YourEnum::REALM)))) };
  return *instance;
}


::std::string_view _fbthrift_schema_e062e2c29ccabf8a() {
  return "";
}
::folly::Range<const ::std::string_view*> _fbthrift_schema_e062e2c29ccabf8a_includes() {
  return {};
}
::folly::Range<const ::std::string_view*> _fbthrift_schema_e062e2c29ccabf8a_uris() {
  return {};
}

} // namespace module_constants
} // namespace cpp2
