include_guard(GLOBAL)

include(src/util/termcolor2/Mod.cmake)
include(src/util/semver/Mod.cmake)

add_library(poac_util_rustify)
target_sources(poac_util_rustify
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/rustify.ixx
)

# When using result-macros.hpp, we need to link to this interface library.
add_library(poac_util_result_macros INTERFACE)
target_link_libraries(poac_util_result_macros INTERFACE
    fmt::fmt # mitama-cpp-result depends on fmt internally
    mitama-cpp-result::mitama-cpp-result
)
add_library(poac_util_result)
target_sources(poac_util_result
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/result.ixx
)
target_link_libraries(poac_util_result PRIVATE
    poac_util_result_macros
)

add_library(poac_util_cfg)
target_sources(poac_util_cfg
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/cfg.ixx
)
target_link_libraries(poac_util_cfg PRIVATE
    poac_util_rustify
)

add_library(poac_util_levDistance)
target_sources(poac_util_levDistance
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/levDistance.ixx
)
target_link_libraries(poac_util_levDistance PRIVATE
    poac_util_rustify
)

add_library(poac_util_misc)
target_sources(poac_util_misc
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/misc.ixx
)
target_link_libraries(poac_util_misc PRIVATE poac_util_rustify poac_util_result)

add_library(poac_util_file)
target_sources(poac_util_file
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/file.ixx
)
target_link_libraries(poac_util_file PRIVATE
    poac_util_result
    poac_util_rustify
)

add_library(poac_util_verbosity)
target_sources(poac_util_verbosity
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/verbosity.ixx
)
target_link_libraries(poac_util_verbosity PRIVATE
    spdlog::spdlog
)

add_library(poac_util_format)
target_sources(poac_util_format
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/format.ixx
)
target_link_libraries(poac_util_format PRIVATE
    fmt::fmt
)

add_library(poac_util_pretty)
target_sources(poac_util_pretty
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/pretty.ixx
)
target_link_libraries(poac_util_pretty PRIVATE
    poac_util_format
    poac_util_rustify
)

add_library(poac_util_meta)
target_sources(poac_util_meta
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/meta.ixx
)
target_link_libraries(poac_util_meta PRIVATE
    poac_util_rustify
)

add_library(poac_util_shell)
target_sources(poac_util_shell
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/shell.ixx
)

add_library(poac_util_log)
target_sources(poac_util_log
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/log.ixx
)
target_link_libraries(poac_util_log PRIVATE
    poac_util_format

    spdlog::spdlog
    termcolor2
    termcolor2_literals_extra
    termcolor2_to_color_extra
)

add_library(poac_util_sha256)
target_sources(poac_util_sha256
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/sha256.ixx
)
target_link_libraries(poac_util_sha256 PRIVATE
    poac_util_format
    poac_util_log
    poac_util_result
    poac_util_rustify
)

add_library(poac_util_archive)
target_sources(poac_util_archive
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/archive.ixx
)
target_link_libraries(poac_util_archive PRIVATE
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
)
