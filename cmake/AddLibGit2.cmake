include_guard(GLOBAL)

message(CHECK_START "Adding libgit2")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        libgit2
        GIT_REPOSITORY https://github.com/libgit2/libgit2.git
        GIT_TAG        v1.1.1
)

set(CMAKE_PROJECT_libgit2_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/CMP0077PolicyFix.cmake")
set(BUILD_CLAR OFF) # Disable testing
if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(BUILD_SHARED_LIBS OFF)
endif ()
FetchContent_MakeAvailable(libgit2)
unset(BUILD_CLAR)
unset(BUILD_SHARED_LIBS)

list(APPEND POAC_DEPENDENCIES git2)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
