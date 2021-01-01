include_guard(GLOBAL)

# Check required compiler version
# https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    execute_process(
            COMMAND bash -c "sw_vers | head -n 2 | tail -n 1 | cut -f 2"
            OUTPUT_VARIABLE MACOS_VERSION)
    if (${MACOS_VERSION} VERSION_LESS 10.15)
        message(FATAL_ERROR "Poac requires macOS Catalina (10.15) or later.")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # https://cmake.org/cmake/help/latest/variable/MSVC_TOOLSET_VERSION.html
    if (${MSVC_TOOLSET_VERSION} LESS 141)
        message(FATAL_ERROR "Poac requires Visual Studio 2017 or later.")
    endif()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
        message(FATAL_ERROR "Poac requires g++-8 or later.")
    endif ()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7)
        message(FATAL_ERROR "Poac requires clang++-7 or later.")
    endif ()
endif ()
