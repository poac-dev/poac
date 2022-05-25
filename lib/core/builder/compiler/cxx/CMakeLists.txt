add_library(poac_core_builder_compiler_cxx
    apple_clang.cc
    clang.cc
    gcc.cc
)

target_compile_features(poac_core_builder_compiler_cxx PUBLIC cxx_std_20)
target_include_directories(poac_core_builder_compiler_cxx PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
)
target_link_libraries(poac_core_builder_compiler_cxx PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
    poac_core_builder_compiler_lang
    poac_util
    poac_util_semver
)

add_library(poac::core::builder::compiler::cxx ALIAS poac_core_builder_compiler_cxx)
