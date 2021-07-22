include_guard(GLOBAL)

message(CHECK_START "Adding fmt")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        7.1.3
)

if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(BUILD_SHARED_LIBS OFF)
endif ()
FetchContent_MakeAvailable(fmt)
unset(BUILD_SHARED_LIBS)

list(APPEND POAC_DEPENDENCIES fmt::fmt)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
