add_executable(gen-type-scanners gen-type-scanners.cpp)

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  target_compile_definitions(gen-type-scanners PRIVATE "CLANG_STANDALONE_DEBUG")
endif()

target_link_libraries(gen-type-scanners PRIVATE hphp_debug_parser hphp_util tbb Folly::folly glog)
