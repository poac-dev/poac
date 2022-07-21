add_poac_target(poac_util_archive
    SOURCES archive.cc
    INCLUDES
        ${POAC_HPP_INCLUDES}
        ${LIBARCHIVE_INCLUDE_DIR}
    LIBRARIES
        ${POAC_HPP_LIBS}
        ${LIBARCHIVE_LIBRARY}
)

add_poac_target(poac_util_cfg
    SOURCES cfg.cc
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES ${POAC_HPP_LIBS}
)

add_poac_target(poac_util_lev_distance
    SOURCES lev_distance.cc
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES ${POAC_HPP_LIBS}
)

add_poac_target(poac_util_meta
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES ${POAC_HPP_LIBS}
)

add_poac_target(poac_util_misc
    SOURCES misc.cc
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES ${POAC_HPP_LIBS}
)

add_poac_target(poac_util_net
    SOURCES net.cc
    INCLUDES
        ${POAC_HPP_INCLUDES}
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
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES ${POAC_HPP_LIBS}
)

add_poac_target(poac_util_sha256
    SOURCES sha256.cc
    INCLUDES
        ${POAC_HPP_INCLUDES}
        ${OPENSSL_INCLUDE_DIR}
    LIBRARIES
        ${POAC_HPP_LIBS}
        ${OPENSSL_LIBRARIES}
)

add_poac_target(poac_util_shell
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES ${POAC_HPP_LIBS}
)

add_poac_target(poac_util_verbosity
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES ${POAC_HPP_LIBS}
)

add_subdirectory(semver)
