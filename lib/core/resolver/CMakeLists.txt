add_poac_target(poac_core_resolver
    SOURCES
        sat.cc
        resolve.cc
    INCLUDES ${OPENSSL_INCLUDE_DIR}
    LIBRARIES
        poac_util_net
        poac_util_semver
        poac_util_misc
        poac_util_pretty
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
        poac_util_registry_conan_resolver
)
