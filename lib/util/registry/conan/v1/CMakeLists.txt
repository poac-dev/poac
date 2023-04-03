add_poac_target(poac_util_registry_conan_v1_resolver
    SOURCES resolver.cc
    LIBRARIES
        toml11::toml11
        poac_util_cfg
        poac_util_file
        poac_util_log
        poac_util_result
        poac_util_rustify
        poac_util_shell
)

add_poac_target(poac_util_registry_conan_v1_manifest
    SOURCES manifest.cc
    INCLUDES
        ${Boost_INCLUDE_DIRS}
    LIBRARIES
        toml11::toml11
        poac_util_format
        poac_util_result
        poac_util_rustify
)
