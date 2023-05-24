include_guard(GLOBAL)

add_library(semver_exception)
target_sources(semver_exception
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/exception.ixx
)

add_library(semver_token)
target_sources(semver_token
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/token.ixx
)

add_library(semver_range)
target_sources(semver_range
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/range.ixx
)
target_link_libraries(semver_range PRIVATE
    semver_token
)

add_library(semver_lexer)
target_sources(semver_lexer
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/lexer.ixx
)
target_link_libraries(semver_lexer PRIVATE
    semver_token
)

add_library(semver_parser)
target_sources(semver_parser
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/parser.ixx
)
target_link_libraries(semver_parser PRIVATE
    semver_exception
    semver_lexer
    semver_token
    semver_range
)

add_library(semver_comparison)
target_sources(semver_comparison
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/comparison.ixx
)
target_link_libraries(semver_comparison PRIVATE
    semver_lexer
    semver_parser
    semver_token
)

add_library(semver_interval)
target_sources(semver_interval
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/interval.ixx
)
target_link_libraries(semver_interval PRIVATE
    fmt::fmt
    semver_comparison
    semver_exception
    semver_parser
    semver_token
)

add_library(semver_io)
target_sources(semver_io
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/io.ixx
)
target_link_libraries(semver_io PRIVATE
    fmt::fmt
    semver_token
)

add_library(semver)
target_sources(semver
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/semver/semver.ixx
)
target_link_libraries(semver PRIVATE
    semver_comparison
    semver_exception
    semver_interval
    semver_io
    semver_lexer
    semver_parser
    semver_range
    semver_token
)
