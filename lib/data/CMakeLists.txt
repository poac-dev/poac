add_poac_target(poac_data
    SOURCES lockfile.cc
    DEFINES TOML11_NO_ERROR_PREFIX
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES
        ${POAC_HPP_LIBS}
        toml11::toml11
        poac_core_resolver
)
