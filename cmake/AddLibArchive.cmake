include_guard(GLOBAL)

message(CHECK_START "Adding LibArchive")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(cmake/CPM.cmake)

if (APPLE)
    # To support finding LibArchive on macOS
    set(LibArchive_INCLUDE_DIR "${POAC_HOMEBREW_ROOT_PATH}/libarchive/include")
endif ()

set(CMAKE_PROJECT_libarchive_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/LibArchivePoliciesFix.cmake")
CPMAddPackage(
    NAME LibArchive
    GITHUB_REPOSITORY libarchive/libarchive
    VERSION 3.6.0 # minimum required version (but only when using local one)
    GIT_TAG v3.6.1 # (3.6.1 or over is needed to build downloaded one)
    OPTIONS
        "ENABLE_OPENSSL OFF"
        "ENABLE_LIBB2 OFF"
        "ENABLE_LZMA OFF"
        "ENABLE_ZSTD OFF"
        "ENABLE_ZLIB OFF"
        "ENABLE_BZip2 OFF"
        "ENABLE_LIBXML2 OFF"
        "ENABLE_EXPAT OFF"
        "ENABLE_CAT OFF"
        "ENABLE_CPIO OFF"
        "ENABLE_TAR OFF"
        "ENABLE_ACL OFF"
        "ENABLE_ICONV OFF"
        "ENABLE_TEST OFF"
        "ENABLE_INSTALL OFF"
)

if (LibArchive_ADDED) # If using downloaded package
    set(LIBARCHIVE_INCLUDE_DIR "${LibArchive_SOURCE_DIR}/libarchive")
    if (CMAKE_BUILD_TYPE STREQUAL Debug) # -DCMAKE_BUILD_TYPE=Debug
        set(LIBARCHIVE_LIBRARY archive)
        list(APPEND POAC_DEPENDENCIES archive)
    else ()
        set(LIBARCHIVE_LIBRARY archive_static)
        list(APPEND POAC_DEPENDENCIES archive_static)
    endif ()
else () # If using local package
    if (APPLE)
        set(LIBARCHIVE_INCLUDE_DIR ${LibArchive_INCLUDE_DIR})
    endif ()
    set(LIBARCHIVE_LIBRARY archive)
    list(APPEND POAC_DEPENDENCIES archive)
endif ()

message(CHECK_PASS "added")
list(POP_BACK CMAKE_MESSAGE_INDENT)
