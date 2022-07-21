add_poac_target(poac_util_semver
    CXX 17
    SOURCES
        comparison.cc
        interval.cc
        lexer.cc
        parser.cc
        token.cc
    INCLUDES ${CMAKE_HOME_DIRECTORY}/include
    LIBRARIES fmt::fmt
)
