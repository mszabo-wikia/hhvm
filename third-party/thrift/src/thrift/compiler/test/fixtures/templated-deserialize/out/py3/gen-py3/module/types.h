/**
 * Autogenerated by Thrift for thrift/compiler/test/fixtures/templated-deserialize/src/module.thrift
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */

#pragma once

#include <functional>
#include <folly/Range.h>

#include "thrift/compiler/test/fixtures/templated-deserialize/gen-cpp2/module_data.h"
#include "thrift/compiler/test/fixtures/templated-deserialize/gen-cpp2/module_types.h"
#include "thrift/compiler/test/fixtures/templated-deserialize/gen-cpp2/module_metadata.h"
namespace thrift {
namespace py3 {



template<>
inline void reset_field<::cpp2::SmallStruct>(
    ::cpp2::SmallStruct& obj, uint16_t index) {
  switch (index) {
    case 0:
      obj.small_A_ref().copy_from(default_inst<::cpp2::SmallStruct>().small_A_ref());
      return;
    case 1:
      obj.small_B_ref().copy_from(default_inst<::cpp2::SmallStruct>().small_B_ref());
      return;
  }
}

template<>
inline void reset_field<::cpp2::containerStruct>(
    ::cpp2::containerStruct& obj, uint16_t index) {
  switch (index) {
    case 0:
      obj.fieldA_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldA_ref());
      return;
    case 1:
      obj.fieldB_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldB_ref());
      return;
    case 2:
      obj.fieldC_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldC_ref());
      return;
    case 3:
      obj.fieldD_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldD_ref());
      return;
    case 4:
      obj.fieldE_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldE_ref());
      return;
    case 5:
      obj.fieldF_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldF_ref());
      return;
    case 6:
      obj.fieldG_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldG_ref());
      return;
    case 7:
      obj.fieldH_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldH_ref());
      return;
    case 8:
      obj.fieldI_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldI_ref());
      return;
    case 9:
      obj.fieldJ_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldJ_ref());
      return;
    case 10:
      obj.fieldK_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldK_ref());
      return;
    case 11:
      obj.fieldL_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldL_ref());
      return;
    case 12:
      obj.fieldM_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldM_ref());
      return;
    case 13:
      obj.fieldQ_ref().copy_from(default_inst<::cpp2::containerStruct>().fieldQ_ref());
      return;
    case 14:
      obj.fieldR_ref().reset();
      return;
    case 15:
      obj.fieldS_ref().reset();
      return;
    case 16:
      obj.fieldT_ref().reset();
      return;
    case 17:
      obj.fieldU_ref().reset();
      return;
    case 18:
      obj.fieldX_ref().reset();
      return;
  }
}

template<>
inline const std::unordered_map<std::string_view, std::string_view>& PyStructTraits<
    ::cpp2::SmallStruct>::namesmap() {
  static const folly::Indestructible<NamesMap> map {
    {
    }
  };
  return *map;
}

template<>
inline const std::unordered_map<std::string_view, std::string_view>& PyStructTraits<
    ::cpp2::containerStruct>::namesmap() {
  static const folly::Indestructible<NamesMap> map {
    {
    }
  };
  return *map;
}
} // namespace py3
} // namespace thrift
