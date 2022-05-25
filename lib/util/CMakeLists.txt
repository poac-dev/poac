add_library(poac_util
    archive.cc
    cfg.cc
    lev_distance.cc
    misc.cc
    net.cc
    pretty.cc
    sha256.cc
    shell.cc
)

target_compile_features(poac_util PUBLIC cxx_std_20)
target_include_directories(poac_util PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${LIBARCHIVE_INCLUDE_DIR}
)
target_link_libraries(poac_util PUBLIC
    ${STATIC_LINK_FLAG}
    ${LIBARCHIVE_LIBRARY}
    ${OPENSSL_LIBRARIES}
    ${POAC_HPP_LIBS}
)

add_library(poac::util ALIAS poac_util)

add_subdirectory(semver)
