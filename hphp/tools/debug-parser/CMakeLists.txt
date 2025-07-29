auto_sources(CXX_SOURCES "*.cpp" "")
add_library(hphp_debug_parser ${CXX_SOURCES})

target_link_libraries(hphp_debug_parser PUBLIC Folly::folly tbb LLVMDebugInfoDWARF)
