add_library(mcrouter INTERFACE)

include(ExternalProject)
include(HPHPFunctions)

set(
  MCROUTER_DEPS
  boost
  fizz
  fmt
  folly
  jemalloc
  thrift
  wangle
)
add_library(mcrouter_deps INTERFACE)
target_link_libraries(mcrouter_deps INTERFACE ${MCROUTER_DEPS})
add_dependencies(mcrouter_deps ragel)

ExternalProject_Add(
  bundled_mcrouter
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/
  BUILD_ALWAYS ON
  PATCH_COMMAND
    "${CMAKE_COMMAND}" -E create_symlink
    "${CMAKE_CURRENT_SOURCE_DIR}/mcrouter-CMakeLists.txt"
    "CMakeLists.txt"
    &&
    "${CMAKE_COMMAND}" -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/config.h"
    "mcrouter/"
    &&
    "${CMAKE_COMMAND}" -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/config-impl.h"
    "mcrouter/"
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_INSTALL_INCLUDEDIR=include
    -DCMAKE_INSTALL_LIBDIR=lib
    -DCMAKE_MODULE_PATH=${CMAKE_SOURCE_DIR}/build/fbcode_builder/CMake

    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
    -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}

    "-DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}"
    "-DBOOST_INCLUDE_DIR=$<TARGET_PROPERTY:boost,INTERFACE_INCLUDE_DIRECTORIES>"
    "-DGLOG_INCLUDE_DIR=${GLOG_INCLUDE_DIR}"
    "-DJEMALLOC_INCLUDE_DIR=$<TARGET_PROPERTY:jemalloc,INTERFACE_INCLUDE_DIRECTORIES>"
    "-DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}"

    "-Dragel_EXECUTABLE=$<TARGET_FILE:ragel>"

    "-Dfmt_DIR=${fmt_DIR}"
    "-Dfolly_DIR=${FOLLY_INSTALL_DIR}/lib/cmake/folly"
    "-Dfizz_DIR=${FIZZ_INSTALL_DIR}/lib/cmake/fizz"
    "-DFizz_DIR=${FIZZ_INSTALL_DIR}/lib/cmake/fizz"
    "-DFBThrift_DIR=${THRIFT_INSTALL_DIR}/lib/cmake/fbthrift"
    "-Dwangle_DIR=${WANGLE_INSTALL_DIR}/lib/cmake/wangle"
    "-Dmvfst_DIR=${MVFST_INSTALL_DIR}/lib/cmake/mvfst"
  DEPENDS ${MCROUTER_DEPS} ragel
  EXCLUDE_FROM_ALL
)

add_dependencies(mcrouter bundled_mcrouter)
ExternalProject_Get_Property(bundled_mcrouter INSTALL_DIR)
ExternalProject_Get_Property(bundled_mcrouter SOURCE_DIR)
ExternalProject_Get_Property(bundled_mcrouter BINARY_DIR)
target_include_directories(
  mcrouter
  INTERFACE
  "${SOURCE_DIR}"
  "${BINARY_DIR}"
  "${BINARY_DIR}/thrift-gen/src"
)

list(APPEND MCROUTER_INSTALL_TARGETS
  mcrouter
  mcrouter_memcache_service_thrift
  mcrouter_memcache_thrift
  mcrouter_common_thrift
  mcrouter_carbon_result_thrift
  mcrouter_carbon_thrift
)

foreach(target ${MCROUTER_INSTALL_TARGETS})
  target_link_libraries(
    mcrouter
    INTERFACE
    "${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}${target}${CMAKE_STATIC_LIBRARY_SUFFIX}"
  )
endforeach()

target_link_libraries(
  mcrouter
  INTERFACE
  mcrouter_deps
)
