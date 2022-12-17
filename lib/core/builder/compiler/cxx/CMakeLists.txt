add_poac_target(poac_core_builder_compiler_cxx
    SOURCES
        apple_clang.cc
        clang.cc
        cxx.cc
        gcc.cc
    LIBRARIES
        poac_core_builder_compiler
        poac_util_cfg
        poac_util_semver
        poac_util_misc # in cxx.cc
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)
