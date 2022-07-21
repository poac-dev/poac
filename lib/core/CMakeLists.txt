add_poac_target(poac_core
    SOURCES
        resolver.cc
        validator.cc
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES
        ${POAC_HPP_LIBS}
        poac_core_builder_ninja
        poac_core_resolver
        poac_data
        poac_util_archive
        poac_util_sha256
        poac_util_semver
        poac_util_misc # via poac/config.hpp
        toml11::toml11
)

add_subdirectory(builder)
add_subdirectory(resolver)
