include_guard(GLOBAL)

message(CHECK_START "Adding Spdlog")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(cmake/CPM.cmake)

if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(SPDLOG_BUILD_SHARED OFF)
else ()
    set(SPDLOG_BUILD_SHARED ON)
endif ()
CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.9.0 # minimum required version
    GIT_TAG v1.10.0
    OPTIONS
        "SPDLOG_FMT_EXTERNAL ON"
        "SPDLOG_BUILD_SHARED ${SPDLOG_BUILD_SHARED}"
)

list(APPEND POAC_DEPENDENCIES spdlog::spdlog)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
