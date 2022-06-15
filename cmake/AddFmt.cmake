include_guard(GLOBAL)

message(CHECK_START "Adding fmt")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(cmake/CPM.cmake)

if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(BUILD_SHARED_LIBS OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
endif ()
CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 8.1.1
    OPTIONS
        "BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS}"
)

list(APPEND POAC_DEPENDENCIES fmt::fmt)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
