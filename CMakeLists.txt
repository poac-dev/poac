cmake_minimum_required(VERSION 3.0)
project(poac)

# Preprocessor definitions
add_definitions( -DPOAC_PROJECT_ROOT="${CMAKE_SOURCE_DIR}" )
add_definitions( -DPOAC_VERSION="0.1.2" )
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color -Wall -Wextra -Werror")
endif()

# Check of c++17
if(WIN32)
    include(CheckCXXCompilerFlag)
    CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
    if (_cpp_latest_flag_supported)
        add_compile_options("/std:c++latest")
    endif()
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
elseif(LINUX)
    set(STATIC_LINK_FLAG "-static")
elseif(WIN32)
    include( "C:/Libraries/boost_1_69_0" )
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME}
        "${STATIC_LINK_FLAG}"
        "boost_filesystem"
        "boost_system"
        "ssl"
        "crypto"
        "dl"
        "yaml-cpp")
install(TARGETS poac DESTINATION bin)
