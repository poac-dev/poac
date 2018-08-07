cmake_minimum_required(VERSION 3.6)
project(poac)

# Preprocessor definitions
add_definitions( -DPOAC_ROOT="${CMAKE_SOURCE_DIR}" )
add_definitions( -DPOAC_VERSION="0.0.1" )
add_definitions( -fdiagnostics-color -Wall -Werror -Wextra )

# Check of c++17
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

# Boost
find_package(Boost 1.62 REQUIRED COMPONENTS system filesystem timer)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIR})
else()
    message(WARNING "The Boost was not found.")
endif ()

# CURL
set(CURL_LIBRARY "-lcurl")
add_definitions(-DCURL_STATICLIB)
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# yaml-cpp
find_package( yaml-cpp REQUIRED )
include_directories( ${YAML_CPP_INCLUDE_DIR} )

# libgit2
#include_directories( ${CMAKE_SOURCE_DIR}/deps/libgit2-0.27.3/include )
#link_directories( ${CMAKE_SOURCE_DIR}/deps/libgit2-0.27.3/lib )

add_subdirectory(src)
add_subdirectory(test)
enable_testing()
add_test(NAME poac_test
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        COMMAND unittest)
