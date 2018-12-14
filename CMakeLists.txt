cmake_minimum_required(VERSION 3.0)
project(poac)

# Preprocessor definitions
add_definitions( -DPOAC_AUTO_DEF_PROJECT_ROOT="${CMAKE_SOURCE_DIR}" )
add_definitions( -DPOAC_VERSION="0.3.0-beta" )
add_definitions( -fdiagnostics-color -Wall -Wextra ) # -Werror

# Check of c++17
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
if(COMPILER_SUPPORTS_CXX17)
    message(STATUS "XCODE_VERSION: $ENV{XCODE_VERSION}")
    if($ENV{XCODE_VERSION} LESS 9.3)
        message(STATUS "CXX_FLAG: -std=c++1z")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -pthread")
    else()
        message(STATUS "CXX_FLAG: -std=c++17")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
    endif()
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
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

# CVC4
include_directories( "/usr/local/include" )
link_directories( "/usr/local/lib" )

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_TIMER_LIBRARY}
        ${Boost_CHRONO_LIBRARY}
        ${CURL_LIBRARIES}
        ${YAML_CPP_LIBRARIES}
        "cvc4"
        # https://stackoverflow.com/questions/48411537/cvc4-mkconst-from-stdstring-in-c-api
        "gmp"
        )
install(TARGETS poac DESTINATION bin)
