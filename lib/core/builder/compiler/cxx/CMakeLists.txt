add_poac_target(poac_core_builder_compiler_cxx
    SOURCES
        apple_clang.cc
        clang.cc
        cxx.cc
        gcc.cc
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES
        ${POAC_HPP_LIBS}
        poac_core_builder_compiler_lang
        poac_util_cfg
        poac_util_semver
        poac_util_misc # in cxx.cc
)
