add_poac_target(poac_util_archive
    SOURCES archive.cc
    INCLUDES ${LIBARCHIVE_INCLUDE_DIR}
    LIBRARIES
        ${LIBARCHIVE_LIBRARY}
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)

add_poac_target(poac_util_cfg
    SOURCES cfg.cc
    LIBRARIES poac_util_rustify
)

add_poac_header_target(poac_util_format
    LIBRARIES fmt::fmt
)

add_poac_target(poac_util_lev_distance
    SOURCES lev_distance.cc
    LIBRARIES poac_util_rustify
)

add_poac_header_target(poac_util_log
    LIBRARIES spdlog::spdlog
)

add_poac_target(poac_util_meta
    LIBRARIES poac_util_rustify
)

add_poac_target(poac_util_misc
    SOURCES misc.cc
    LIBRARIES
        poac_util_result
        poac_util_rustify
)

add_poac_target(poac_util_net
    SOURCES net.cc
    INCLUDES
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
    LIBRARIES
        ${OPENSSL_LIBRARIES}
        poac_util_misc
        poac_util_pretty
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)

add_poac_target(poac_util_pretty
    SOURCES pretty.cc
    LIBRARIES
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)

add_poac_header_target(poac_util_result
    LIBRARIES
        fmt::fmt # mitama-cpp-result uses `fmt` internally
        mitama-cpp-result::mitama-cpp-result
)

add_poac_header_target(poac_util_rustify
    INCLUDES
        ${CMAKE_HOME_DIRECTORY}/include
        ${Boost_INCLUDE_DIRS}
)

add_poac_target(poac_util_sha256
    SOURCES sha256.cc
    INCLUDES
        ${OPENSSL_INCLUDE_DIR}
    LIBRARIES
        ${OPENSSL_LIBRARIES}
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)

add_poac_target(poac_util_shell
    LIBRARIES
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)

add_poac_target(poac_util_validator
    SOURCES validator.cc
    LIBRARIES
        poac_util_semver
        poac_util_misc # via poac/config.hpp
        poac_util_rustify # via poac/config.hpp
        poac_util_format
        poac_util_log
        poac_util_result
        toml11::toml11
)

add_poac_target(poac_util_verbosity
    LIBRARIES
        poac_util_format
        poac_util_log
        poac_util_result
        poac_util_rustify
)

add_subdirectory(semver)
