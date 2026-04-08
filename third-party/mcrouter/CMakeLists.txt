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

get_target_property(BOOST_INCLUDE_DIR boost INTERFACE_INCLUDE_DIRECTORIES)
get_target_property(BOOST_LIBRARIES boost INTERFACE_LINK_LIBRARIES)
list(GET BOOST_LIBRARIES 0 FIRST_LIB)
if("${FIRST_LIB}" MATCHES ".+/${CMAKE_STATIC_LIBRARY_PREFIX}boost_.+${CMAKE_STATIC_LIBRARY_SUFFIX}$")
  set(Boost_USE_STATIC_LIBS ON)
else()
  set(Boost_USE_STATIC_LIBS OFF)
endif()
get_filename_component(BOOST_LIBRARY_DIR "${FIRST_LIB}" DIRECTORY)

ExternalProject_Add(
  bundled_mcrouter
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/
  BUILD_ALWAYS ON
  PATCH_COMMAND
    ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/CMake/FindBoost.cmake
    "CMake/"
  LIST_SEPARATOR "!!"
  CMAKE_ARGS
    "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}!!${GLOG_INSTALL_DIR}!!${GFLAGS_INSTALL_DIR}"
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
    "-DBoost_INCLUDE_DIR=$<TARGET_PROPERTY:boost,INTERFACE_INCLUDE_DIRECTORIES>"
    "-DBOOST_LIBRARYDIR=${BOOST_LIBRARY_DIR}"
    -DBoost_NO_SYSTEM_PATHS=ON
    -DBoost_NO_BOOST_CMAKE=ON
    -DBoost_USE_STATIC_LIBS=${Boost_USE_STATIC_LIBS}
    "-DJEMALLOC_INCLUDE_DIR=$<TARGET_PROPERTY:jemalloc,INTERFACE_INCLUDE_DIRECTORIES>"
    "-DOPENSSL_INCLUDE_DIR=${OPENSSL_INCLUDE_DIR}"

    "-Dragel_EXECUTABLE=$<TARGET_FILE:ragel>"

    "-Dgflags_DIR=${gflags_DIR}"

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
  mcroutercore
  mcrouter
  mcrouter_memcache_service_thrift
  mcrouter_memcache_thrift
  mcrouter_carbon_result_thrift
  mcrouter_carbon_thrift
  mcrouter_common_thrift
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
