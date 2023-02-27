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
    VERSION 8.1.0 # minimum required version
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 9.1.0
    OPTIONS
        "BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS}"
)

if (POAC_CLANG_TIDY AND fmt_ADDED) # If using downloaded package
    # Disable clang-tidy
    set_target_properties(
        fmt
        PROPERTIES
            CXX_CLANG_TIDY ""
    )
endif ()

list(APPEND POAC_gDEPENDENCIES fmt::fmt)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
