add_library(poac_util_semver
    comparison.cc
    interval.cc
    lexer.cc
    parser.cc
    token.cc
)
target_compile_features(poac_util_semver PUBLIC cxx_std_17)
target_include_directories(poac_util_semver PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
)
target_link_libraries(poac_util_semver PUBLIC
    ${STATIC_LINK_FLAG}
    fmt::fmt
)

add_library(poac::util::semver ALIAS poac_util_semver)
