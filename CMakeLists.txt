cmake_minimum_required(VERSION 3.9)
project(poac)

# preprocessor definitions
add_definitions( -DPOAC_ROOT="${CMAKE_SOURCE_DIR}" )
add_definitions( -fdiagnostics-color -Wall -Werror -Wextra )

# Check of c++17
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

file(GLOB_RECURSE SOURCES libcpp/*)

# Boost
find_package(Boost 1.66 REQUIRED COMPONENTS system filesystem)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIR})
else()
    message(WARNING "The Boost was not found.")
endif ()

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
