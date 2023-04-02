include_guard(GLOBAL)

message(CHECK_START "Adding libgit2")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

include(cmake/CPM.cmake)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Override cache string in libgit2/CMakeLists.txt
# ref: https://github.com/libgit2/libgit2/blob/43b5075df4c543fc8801ed4b829702d7f9f2c0ad/CMakeLists.txt#L57
# ref2: https://github.com/libgit2/libgit2/blob/43b5075df4c543fc8801ed4b829702d7f9f2c0ad/src/CMakeLists.txt#L163
# ref for how to do this: https://stackoverflow.com/a/62702897
set(REGEX_BACKEND "builtin" CACHE INTERNAL "")

set(CMAKE_PROJECT_libgit2_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/CMP0077PolicyFix.cmake")
if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(BUILD_SHARED_LIBS OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
endif ()
CPMAddPackage(
    NAME libgit2
    GITHUB_REPOSITORY libgit2/libgit2
    VERSION 1.4.3 # minimum required version
    GIT_TAG v1.6.1
    OPTIONS
        "BUILD_TESTS OFF"
        "USE_SSH OFF"
        "USE_HTTPS OFF"
        "USE_NTLMCLIENT OFF"
        "USE_BUNDLED_ZLIB ON"
        "BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS}"
)

if (libgit2_ADDED) # If using downloaded package
    set(LIBGIT2_INCLUDE_DIR ${libgit2_SOURCE_DIR}/include)
    set(LIBGIT2_LIBRARY libgit2package)
else () # If using local package
    set(LIBGIT2_LIBRARY ${LIBGIT2_LIBRARIES})
endif ()

list(APPEND POAC_DEPENDENCIES ${LIBGIT2_LIBRARY})
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
