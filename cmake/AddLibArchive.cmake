include_guard(GLOBAL)

message(CHECK_START "Adding LibArchive")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        libarchive
        GIT_REPOSITORY https://github.com/libarchive/libarchive.git
        GIT_TAG        v3.6.1
)

set(ENABLE_OPENSSL OFF)
set(ENABLE_LZMA OFF)

set(ENABLE_LIBXML2 OFF)
set(ENABLE_EXPAT OFF)

# Prevent building unused executables
set(ENABLE_CAT OFF)
set(ENABLE_CPIO OFF)
set(ENABLE_TAR OFF)

set(ENABLE_ACL OFF)
set(ENABLE_ICONV OFF)
set(ENABLE_TEST OFF)
set(ENABLE_INSTALL OFF)

set(CMAKE_PROJECT_libarchive_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/LibArchivePoliciesFix.cmake")
FetchContent_MakeAvailable(libarchive)

if (APPLE)
  set(LIBARCHIVE_LIBRARY archive)
else ()
  if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(LIBARCHIVE_LIBRARY_NAME libarchive.a)
  else ()
    set(LIBARCHIVE_LIBRARY_NAME libarchive.so)
  endif ()
  set(LIBARCHIVE_LIBRARY "${libarchive_BINARY_DIR}/libarchive/${LIBARCHIVE_LIBRARY_NAME}")
  set(LIBARCHIVE_INCLUDE_DIR ${libarchive_SOURCE_DIR}/libarchive)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${LIBARCHIVE_INCLUDE_DIR})
list(APPEND POAC_DEPENDENCIES ${LIBARCHIVE_LIBRARY})

message(CHECK_PASS "added")
list(POP_BACK CMAKE_MESSAGE_INDENT)
