/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/includes/src/includes.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated @nocommit
 */

#include "thrift/compiler/test/fixtures/includes/gen-cpp2/includes_constants.h"

#include <thrift/lib/cpp2/gen/module_constants_cpp.h>

#include "thrift/compiler/test/fixtures/includes/gen-cpp2/transitive_constants.h"

namespace cpp2 {
namespace includes_constants {

::cpp2::Included const& ExampleIncluded() {
  static folly::Indestructible<::cpp2::Included> const instance{ ::apache::thrift::detail::make_structured_constant<::cpp2::Included>(::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::MyIntField>(static_cast<::std::int64_t>(2)), ::apache::thrift::detail::wrap_struct_argument<::apache::thrift::ident::MyTransitiveField>(::cpp2::transitive_constants::ExampleFoo())) };
  return *instance;
}



::std::string_view _fbthrift_schema_708c30a658df2a09() {
  return "";
}
::folly::Range<const ::std::string_view*> _fbthrift_schema_708c30a658df2a09_includes() {
  return {};
}
::folly::Range<const ::std::string_view*> _fbthrift_schema_708c30a658df2a09_uris() {
  return {};
}

} // namespace includes_constants
} // namespace cpp2
