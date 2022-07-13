add_library(poac_util_archive OBJECT archive.cc)
add_library(poac_util_cfg OBJECT cfg.cc)
add_library(poac_util_lev_distance OBJECT lev_distance.cc)
add_library(poac_util_misc OBJECT misc.cc)
add_library(poac_util_pretty OBJECT pretty.cc)
add_library(poac_util_sha256 OBJECT sha256.cc)
add_library(poac_util_net OBJECT net.cc)

target_compile_features(poac_util_archive PUBLIC cxx_std_20)
target_compile_features(poac_util_cfg PUBLIC cxx_std_20)
target_compile_features(poac_util_lev_distance PUBLIC cxx_std_20)
target_compile_features(poac_util_misc PUBLIC cxx_std_20)
target_compile_features(poac_util_pretty PUBLIC cxx_std_20)
target_compile_features(poac_util_sha256 PUBLIC cxx_std_20)
target_compile_features(poac_util_net PUBLIC cxx_std_20)

target_include_directories(poac_util_archive PUBLIC
    ${POAC_HPP_INCLUDES}
    ${LIBARCHIVE_INCLUDE_DIR}
)
target_link_libraries(poac_util_archive PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
    ${LIBARCHIVE_LIBRARY}
)

target_include_directories(poac_util_cfg PUBLIC
    ${POAC_HPP_INCLUDES}
)
target_link_libraries(poac_util_cfg PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
)

target_include_directories(poac_util_lev_distance PUBLIC
    ${POAC_HPP_INCLUDES}
)
target_link_libraries(poac_util_lev_distance PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
)

target_include_directories(poac_util_misc PUBLIC
    ${POAC_HPP_INCLUDES}
)
target_link_libraries(poac_util_misc PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
)

target_include_directories(poac_util_pretty PUBLIC
    ${POAC_HPP_INCLUDES}
)
target_link_libraries(poac_util_pretty PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
)

target_include_directories(poac_util_sha256 PUBLIC
    ${POAC_HPP_INCLUDES}
    ${OPENSSL_INCLUDE_DIR}
)
target_link_libraries(poac_util_sha256 PUBLIC
    ${STATIC_LINK_FLAG}
    ${OPENSSL_LIBRARIES}
    ${POAC_HPP_LIBS}
)

target_include_directories(poac_util_net PUBLIC
    ${POAC_HPP_INCLUDES}
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)
target_link_libraries(poac_util_net PUBLIC
    ${STATIC_LINK_FLAG}
    ${OPENSSL_LIBRARIES}
    poac_util_misc
    poac_util_pretty
    ${POAC_HPP_LIBS}
)

add_library(poac::util::archive ALIAS poac_util_archive)
add_library(poac::util::cfg ALIAS poac_util_cfg)
add_library(poac::util::lev_distance ALIAS poac_util_lev_distance)
add_library(poac::util::misc ALIAS poac_util_misc)
add_library(poac::util::net ALIAS poac_util_net)
add_library(poac::util::pretty ALIAS poac_util_pretty)
add_library(poac::util::sha256 ALIAS poac_util_sha256)

add_subdirectory(semver)
