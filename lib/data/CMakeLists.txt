add_poac_target(poac_data
    SOURCES lockfile.cc
    DEFINES TOML11_NO_ERROR_PREFIX
    LIBRARIES
        toml11::toml11
        poac_core_resolver
        poac_util_result
        poac_util_rustify
)
