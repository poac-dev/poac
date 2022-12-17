add_poac_target(poac_core_builder_compiler
    SOURCES
        error.cc
        lang.cc
    LIBRARIES
        poac_util_cfg
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)

add_subdirectory(cxx)
