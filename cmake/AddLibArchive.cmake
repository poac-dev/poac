include_guard(GLOBAL)

message(CHECK_START "Adding LibArchive")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        libarchive
        GIT_REPOSITORY https://github.com/libarchive/libarchive.git
        GIT_TAG        3.5.1
)

set(CMAKE_PROJECT_libarchive_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/CMP0048PolicyFix.cmake")
set(ENABLE_ACL OFF)
set(ENABLE_ICONV OFF)
set(ENABLE_TEST OFF)
set(ENABLE_INSTALL OFF)

FetchContent_MakeAvailable(libarchive)
list(APPEND POAC_DEPENDENCIES archive)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
