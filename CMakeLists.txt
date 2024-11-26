cmake_minimum_required(VERSION 3.15)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/poac.toml" POAC_TOML_CONTENT)

string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" version_match "${POAC_TOML_CONTENT}")

if(NOT version_match)
    message(FATAL_ERROR "Failed to extract version from poac.toml")
endif()

project(poac VERSION ${version_match} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${CMAKE_GENERATOR}" STREQUAL "Ninja")
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/.vscode/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
    )
endif()

find_package(Git REQUIRED)

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_SHORT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --format=%cd --date=short
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_COMMIT_HASH "unknown")
    set(GIT_COMMIT_SHORT_HASH "unknown")
    set(GIT_COMMIT_DATE "unknown")
endif()

file(GLOB_RECURSE SOURCES "src/*.cc")

# target
add_executable(poac)

target_sources(poac PRIVATE ${SOURCES})

target_compile_definitions(poac PRIVATE
    POAC_POAC_PKG_VERSION="${PROJECT_VERSION}"
    POAC_POAC_COMMIT_HASH="${GIT_COMMIT_HASH}"
    POAC_POAC_COMMIT_SHORT_HASH="${GIT_COMMIT_SHORT_HASH}"
    POAC_POAC_COMMIT_DATE="${GIT_COMMIT_DATE}"
    NOMINMAX
    PCRE2_CODE_UNIT_WIDTH=8
    PCRE2_STATIC
    CURL_STATICLIB
    __TBB_NO_IMPLICIT_LINKAGE
)

# dependencies
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(TBB REQUIRED)
find_package(toml11 REQUIRED)
find_package(CURL REQUIRED)
find_package(unofficial-libgit2 CONFIG REQUIRED)

target_link_libraries(poac PRIVATE
    fmt::fmt
    nlohmann_json::nlohmann_json
    TBB::tbb
    toml11::toml11
    CURL::libcurl
    unofficial::libgit2::libgit2
)
