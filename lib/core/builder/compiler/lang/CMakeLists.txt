add_library(poac_core_builder_compiler_lang
    error.cc
    lang.cc
)

target_compile_features(poac_core_builder_compiler_lang PUBLIC cxx_std_20)
target_include_directories(poac_core_builder_compiler_lang PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
)
target_link_libraries(poac_core_builder_compiler_lang PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
    poac_util_cfg
)
