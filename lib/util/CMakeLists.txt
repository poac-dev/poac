add_poac_target(poac_util_archive
    SOURCES archive.cc
    INCLUDES ${LIBARCHIVE_INCLUDE_DIR}
    LIBRARIES
        ${POAC_HPP_LIBS}
        ${LIBARCHIVE_LIBRARY}
)

add_poac_target(poac_util_cfg
    SOURCES cfg.cc
    LIBRARIES poac_util_rustify
)

add_poac_target(poac_util_lev_distance
    SOURCES lev_distance.cc
    LIBRARIES poac_util_rustify
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
        ${POAC_HPP_LIBS}
)

add_poac_target(poac_util_pretty
    SOURCES pretty.cc
    LIBRARIES ${POAC_HPP_LIBS}
)

add_poac_header_target(poac_util_result
    LIBRARIES mitama-cpp-result::mitama-cpp-result
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
        ${POAC_HPP_LIBS}
        ${OPENSSL_LIBRARIES}
)

add_poac_target(poac_util_shell
    LIBRARIES ${POAC_HPP_LIBS}
)

add_poac_target(poac_util_validator
    SOURCES validator.cc
    LIBRARIES
        ${POAC_HPP_LIBS}
        poac_util_semver
        poac_util_misc # via poac/config.hpp
        poac_util_rustify # via poac/config.hpp
        toml11::toml11
)

add_poac_target(poac_util_verbosity
    LIBRARIES ${POAC_HPP_LIBS}
)

add_subdirectory(semver)
