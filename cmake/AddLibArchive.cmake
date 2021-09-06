include_guard(GLOBAL)

message(CHECK_START "Adding LibArchive")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        libarchive
        GIT_REPOSITORY https://github.com/libarchive/libarchive.git
        GIT_TAG        3.5.1
)

set(ENABLE_OPENSSL OFF)
set(ENABLE_LZMA OFF)

set(ENABLE_LIBXML2 OFF)
set(ENABLE_EXPAT OFF)

set(ENABLE_ACL OFF)
set(ENABLE_ICONV OFF)
set(ENABLE_TEST OFF)
set(ENABLE_INSTALL OFF)

set(CMAKE_PROJECT_libarchive_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/LibArchivePoliciesFix.cmake")
FetchContent_MakeAvailable(libarchive)

if (APPLE)
  list(APPEND POAC_DEPENDENCIES archive)
else ()
  if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(LIBARCHIVE_LIBRARY_NAME libarchive.a)
  else ()
    set(LIBARCHIVE_LIBRARY_NAME libarchive.so)
  endif ()

  target_include_directories(${PROJECT_NAME} PRIVATE ${libarchive_SOURCE_DIR}/libarchive)
  list(APPEND POAC_DEPENDENCIES "${libarchive_BINARY_DIR}/libarchive/${LIBARCHIVE_LIBRARY_NAME}")
endif()

message(CHECK_PASS "added")
list(POP_BACK CMAKE_MESSAGE_INDENT)
