cmake_minimum_required(VERSION 3.0)
project(poac)

# Preprocessor definitions
add_definitions( -DPOAC_AUTO_DEF_PROJECT_ROOT="${CMAKE_SOURCE_DIR}" )
add_definitions( -DPOAC_VERSION="0.3.0-beta" )
add_definitions( -fdiagnostics-color -Wall -Wextra ) # -Werror

# Check of c++17
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1z)
if(COMPILER_SUPPORTS_CXX17)
    if($ENV{XCODE_VERSION})
        message(STATUS "XCODE_VERSION: $ENV{XCODE_VERSION}")
        if($ENV{XCODE_VERSION} LESS 9.3)
            message(STATUS "CXX_FLAG: -std=c++1z")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -pthread")
        else()
            message(STATUS "CXX_FLAG: -std=c++17")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
        endif()
    else()
        message(STATUS "CXX_FLAG: -std=c++17")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
    endif()
elseif(COMPILER_SUPPORTS_CXX1z)
    message(STATUS "CXX_FLAG: -std=c++1z")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -pthread")
else()
    message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
endif()

# CURL
add_definitions(-DCURL_STATICLIB)
find_package(CURL REQUIRED)

include_directories( "/usr/local/include" )
link_directories( "/usr/local/lib" )

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        "/usr/local/lib/libboost_filesystem-mt.a"
        "/usr/local/lib/libboost_system-mt.a"
        "/usr/local/lib/libboost_timer-mt.a"
        "/usr/local/lib/libboost_chrono-mt.a"
        ${CURL_LIBRARIES}
        "yaml-cpp"
        "cvc4"
        "gmp"
        )
install(TARGETS poac DESTINATION bin)
