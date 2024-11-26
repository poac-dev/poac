cmake_minimum_required(VERSION 3.15)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/poac.toml" POAC_TOML_CONTENT)

string(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" version_match "${POAC_TOML_CONTENT}")

if(NOT version_match)
  message(FATAL_ERROR "Failed to extract version from poac.toml")
endif()
project(poac VERSION ${version_match} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# target
add_executable(poac "")
set_target_properties(poac PROPERTIES OUTPUT_NAME "poac")


# dependencies
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(TBB REQUIRED)
find_package(toml11 REQUIRED)
find_package(CURL REQUIRED)
find_package(unofficial-libgit2 CONFIG REQUIRED)

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

target_sources(poac PUBLIC
    src/Algos.cc
    src/BuildConfig.cc
    src/Cli.cc
    src/Cmd/Add.cc
    src/Cmd/Build.cc
    src/Cmd/Clean.cc
    src/Cmd/Fmt.cc
    src/Cmd/Help.cc
    src/Cmd/Init.cc
    src/Cmd/Lint.cc
    src/Cmd/New.cc
    src/Cmd/Run.cc
    src/Cmd/Search.cc
    src/Cmd/Test.cc
    src/Cmd/Tidy.cc
    src/Cmd/Version.cc
    src/Command.cc
    src/CurlVersion.cc
    src/Git2/Commit.cc
    src/Git2/Config.cc
    src/Git2/Describe.cc
    src/Git2/Exception.cc
    src/Git2/Global.cc
    src/Git2/Object.cc
    src/Git2/Oid.cc
    src/Git2/Repository.cc
    src/Git2/Revparse.cc
    src/Git2/Revwalk.cc
    src/Git2/Time.cc
    src/Git2/Version.cc
    src/main.cc
    src/Manifest.cc
    src/Parallelism.cc
    src/Semver.cc
    src/TermColor.cc
    src/VersionReq.cc
)

target_link_libraries(poac PRIVATE
    fmt::fmt
    nlohmann_json::nlohmann_json
    TBB::tbb
    toml11::toml11
    CURL::libcurl
    unofficial::libgit2::libgit2
)
