get_target_property(CARGO_EXE cargo LOCATION)
get_target_property(RUSTC_EXE rustc LOCATION)

add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/hphp/hack/src/hackc/compile/options_gen.h
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/hphp/hack/src/hackc/compile &&
    ${CMAKE_COMMAND} -E env RUSTC=${RUSTC_EXE} ${CARGO_EXE} run --quiet -- hackc ${CMAKE_BINARY_DIR}/hphp/hack/src/hackc/compile ${CMAKE_SOURCE_DIR}/hphp/doc/configs.specification
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${CMAKE_SOURCE_DIR}/hphp/doc/configs.specification rustc cargo
    VERBATIM
)

add_custom_target(hackc_options DEPENDS ${CMAKE_BINARY_DIR}/hphp/hack/src/hackc/compile/options_gen.h)
