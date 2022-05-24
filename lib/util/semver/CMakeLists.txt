add_library(poac_util_semver
    lexer.cc
    parser.cc
    token.cc
)
get_property(POAC_INCLUDE_DIRECTORIES TARGET poac PROPERTY INCLUDE_DIRECTORIES)

target_compile_features(poac_util_semver PUBLIC cxx_std_17)
target_include_directories(poac_util_semver PUBLIC ${CMAKE_HOME_DIRECTORY}/include)
target_link_libraries(poac_util_semver PUBLIC fmt::fmt)

add_library(poac::util::semver ALIAS poac_util_semver)
#list(APPEND POAC_DEPENDENCIES poac::util::semver)
