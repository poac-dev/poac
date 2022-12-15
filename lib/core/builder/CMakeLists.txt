add_subdirectory(compiler)

add_poac_target(poac_core_builder
    SOURCES
        build.cc
        log.cc
        manifest.cc
        syntax.cc
    INCLUDES ${NINJA_INCLUDE_DIR}
    LIBRARIES
        ${NINJA_LIBRARIES}
        Boost::regex
        toml11::toml11
        poac_util_cfg
        poac_util_pretty
        poac_core_builder_compiler_cxx
        poac_core_resolver
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)
