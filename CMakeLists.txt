cmake_minimum_required(VERSION 3.0)
project(poac)

find_package(Boost COMPONENTS system filesystem REQUIRED)

# Preprocessor definitions
add_definitions(-DPOAC_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
add_definitions(-DPOAC_VERSION="0.1.2")
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color -Wall -Wextra -Werror")
endif()

# Check of c++17
if(WIN32 AND MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
#    CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
#    CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1z)
#    if(COMPILER_SUPPORTS_CXX17)
#        message(STATUS "CXX_FLAG: -std=c++17")
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
#    elseif(COMPILER_SUPPORTS_CXX1z)
#        message(STATUS "CXX_FLAG: -std=c++1z")
#        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -pthread")
#    else()
#        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
#    endif()
else()
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
    CHECK_CXX_COMPILER_FLAG("-std=c++1z" COMPILER_SUPPORTS_CXX1z)
    if(COMPILER_SUPPORTS_CXX17)
        message(STATUS "CXX_FLAG: -std=c++17")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -pthread")
    elseif(COMPILER_SUPPORTS_CXX1z)
        message(STATUS "CXX_FLAG: -std=c++1z")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -pthread")
    else()
        message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support.")
    endif()
endif()

include_directories( "/usr/local/include" )
include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( "/usr/local/lib" )
if(APPLE)
    include_directories( "/usr/local/opt/openssl/include" )
    link_directories( "/usr/local/opt/openssl/lib" )
    execute_process(
            COMMAND bash -c "sw_vers | head -n 2 | tail -n 1 | cut -f 2 | cut -f 2 -d \".\""
            OUTPUT_VARIABLE OSX_VERSION
    )
    if(NOT ${OSX_VERSION} EQUAL 14)
        message(STATUS "Set to homebrew's llvm")
        set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/opt/llvm/include")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm/lib")
    endif()
elseif(UNIX)
    if(ENABLE_STATIC) # -DENABLE_STATIC=true
        set(STATIC_LINK_FLAG "-static")
    endif()
elseif(WIN32)
    if(ENABLE_STATIC) # -DENABLE_STATIC=true
        set(STATIC_LINK_FLAG "-static")
    endif()
    find_package(yaml-cpp REQUIRED)
    include_directories( ${YAML_CPP_INCLUDE_DIRS} )

    find_package(OpenSSL REQUIRED)
    include_directories(${OPENSSL_INCLUDE_DIR})
endif()


add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        "${STATIC_LINK_FLAG}"
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        "ssl"
        "crypto"
        "dl"
        "yaml-cpp")

if(WIN32)
    target_link_libraries(${PROJECT_NAME} "-lbcrypt" "-lws2_32")
endif()

if(UNIX AND NOT APPLE)
    add_definitions(-D_GNU_SOURCE -DBOOST_ASIO_HAS_STD_STRING_VIEW)
elseif(CYGWIN)
    add_definitions(-D_GNU_SOURCE -DBOOST_ASIO_HAS_STD_STRING_VIEW -Wa, -mbing-obj)
endif()

install(TARGETS poac DESTINATION bin)
