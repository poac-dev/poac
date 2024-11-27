cmake_minimum_required(VERSION 3.15)

if (UNIX)
    find_program(CLANGPP clang++)
    if (CLANGPP)
        set(CMAKE_CXX_COMPILER ${CLANGPP})
    endif()
endif()


file(READ "${CMAKE_CURRENT_SOURCE_DIR}/poac.toml" POAC_TOML_CONTENT)

string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" version_match "${POAC_TOML_CONTENT}")

if(NOT version_match)
    message(FATAL_ERROR "Failed to extract version from poac.toml")
endif()

project(poac VERSION ${version_match} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(-fdiagnostics-color -pedantic-errors -Wall -Wextra -Wpedantic)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # add_compile_options(/W4 /permissive-)
endif()

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
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cc")

add_library(poac_lib ${SOURCES})

target_compile_definitions(poac_lib PRIVATE
    POAC_POAC_PKG_VERSION="${PROJECT_VERSION}"
    POAC_POAC_COMMIT_HASH="${GIT_COMMIT_HASH}"
    POAC_POAC_COMMIT_SHORT_HASH="${GIT_COMMIT_SHORT_HASH}"
    POAC_POAC_COMMIT_DATE="${GIT_COMMIT_DATE}"
    NOMINMAX
)

# dependencies
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(TBB REQUIRED)
find_package(toml11 REQUIRED)
find_package(CURL REQUIRED)
find_package(unofficial-libgit2 CONFIG REQUIRED)

target_link_libraries(poac_lib PUBLIC
    fmt::fmt
    nlohmann_json::nlohmann_json
    TBB::tbb
    toml11::toml11
    CURL::libcurl
    unofficial::libgit2::libgit2
)

add_executable(poac "src/main.cc")
target_link_libraries(poac PRIVATE poac_lib)

enable_testing()

set(UNITTEST_SRCS
    src/BuildConfig.cc
    src/Algos.cc
    src/Semver.cc
    src/VersionReq.cc
    src/Manifest.cc
)

foreach(TEST_SRC ${UNITTEST_SRCS})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    set(TEST_EXEC "test_${TEST_NAME}")
    add_executable(${TEST_EXEC} ${TEST_SRC})
    target_compile_definitions(${TEST_EXEC} PRIVATE
        POAC_TEST
        NOMINMAX
    )
    target_link_libraries(${TEST_EXEC} PRIVATE
        poac_lib
    )
    add_test(NAME ${TEST_EXEC} COMMAND ${TEST_EXEC})
endforeach()