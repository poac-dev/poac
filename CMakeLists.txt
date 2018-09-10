cmake_minimum_required(VERSION 3.0)
project(poac)

# Preprocessor definitions
add_definitions( -DPOAC_AUTO_DEF_PROJECT_ROOT="${CMAKE_SOURCE_DIR}" )
add_definitions( -DPOAC_VERSION="0.0.1-beta" )
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
find_package(Boost 1.62 REQUIRED COMPONENTS system filesystem timer unit_test_framework)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIR})
else()
    message(WARNING "The Boost was not found.")
endif()

# CURL
add_definitions(-DCURL_STATICLIB)
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# yaml-cpp
find_package( yaml-cpp REQUIRED )
include_directories( ${YAML_CPP_INCLUDE_DIR} )

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_TIMER_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${CURL_LIBRARIES}
        ${YAML_CPP_LIBRARIES}
        )
install(TARGETS poac DESTINATION bin)
