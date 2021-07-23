include_guard(GLOBAL)

message(CHECK_START "Adding LibArchive")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        libarchive
        GIT_REPOSITORY https://github.com/libarchive/libarchive.git
        GIT_TAG        3.5.1
)

set(CMAKE_PROJECT_libarchive_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/LibArchivePoliciesFix.cmake")

set(ENABLE_OPENSSL OFF)
set(ENABLE_LZMA OFF)

set(ENABLE_LIBXML2 OFF)
set(ENABLE_LZMA OFF)

set(ENABLE_ACL OFF)
set(ENABLE_ICONV OFF)
set(ENABLE_TEST OFF)
set(ENABLE_INSTALL OFF)

if (CMAKE_BUILD_TYPE STREQUAL Coverage)
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_BUILD_TYPE_CACHE "Coverage")
endif ()

FetchContent_MakeAvailable(libarchive)
list(APPEND POAC_DEPENDENCIES archive)
message(CHECK_PASS "added")

if (CMAKE_BUILD_TYPE_CACHE STREQUAL Coverage)
    set(CMAKE_BUILD_TYPE "Coverage")
    unset(CMAKE_BUILD_TYPE_CACHE)
endif ()

list(POP_BACK CMAKE_MESSAGE_INDENT)
