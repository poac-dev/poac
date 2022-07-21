add_library(poac_core_resolver
    sat.cc
    resolve.cc
)

target_compile_features(poac_core_resolver PUBLIC cxx_std_20)
target_include_directories(poac_core_resolver PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)
target_link_libraries(poac_core_resolver PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
    poac_util_net
    poac_util_semver
    poac_util_misc
    poac_util_pretty
)
