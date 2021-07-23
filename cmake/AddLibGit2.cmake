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
set(USE_SSH OFF)
set(USE_HTTPS OFF)
set(USE_NTLMCLIENT OFF)
if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(BUILD_SHARED_LIBS OFF)
else ()
    set(BUILD_SHARED_LIBS ON)
endif ()

set(USE_BUNDLED_ZLIB ON)
# Override cache string in libgit2/CMakeLists.txt
# ref: https://github.com/libgit2/libgit2/blob/43b5075df4c543fc8801ed4b829702d7f9f2c0ad/CMakeLists.txt#L57
# ref2: https://github.com/libgit2/libgit2/blob/43b5075df4c543fc8801ed4b829702d7f9f2c0ad/src/CMakeLists.txt#L163
# ref for how to do this: https://stackoverflow.com/a/62702897
set(REGEX_BACKEND "builtin" CACHE INTERNAL "")

FetchContent_MakeAvailable(libgit2)

target_include_directories(${PROJECT_NAME} PRIVATE ${libgit2_SOURCE_DIR}/include)
list(APPEND POAC_DEPENDENCIES git2)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
