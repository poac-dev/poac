add_subdirectory(compiler)
add_subdirectory(ninja)

add_library(poac::core::builder ALIAS poac_core_builder_ninja)
