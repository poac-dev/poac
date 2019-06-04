cmake_minimum_required(VERSION 3.0)
project(poac)

# Preprocessor definitions
add_definitions(-DPOAC_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
add_definitions(-DPOAC_VERSION="0.2.1")
if(CYGWIN)
    add_definitions(-D_GNU_SOURCE -DBOOST_ASIO_HAS_STD_STRING_VIEW -Wa,-mbig-obj)
elseif(UNIX)
    add_definitions("-fdiagnostics-color -Wall -Wextra") # -Werror
    if(NOT APPLE)
        add_definitions(-D_GNU_SOURCE -DBOOST_ASIO_HAS_STD_STRING_VIEW)
    endif()
elseif(WIN32)
    if(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DBOOST_ASIO_HAS_STD_STRING_VIEW /bigobj")
    endif()
endif()


# Check of c++17
if(WIN32 AND MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
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


# Add dependency information
include_directories( "/usr/local/include" )
link_directories( "/usr/local/lib" )

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

if(APPLE)
    include_directories( "/usr/local/opt/openssl/include" )
    link_directories( "/usr/local/opt/openssl/lib" )
    execute_process(
            COMMAND bash -c "sw_vers | head -n 2 | tail -n 1 | cut -f 2 | cut -f 2 -d \".\""
            OUTPUT_VARIABLE OSX_VERSION
    )
    if(NOT ${OSX_VERSION} EQUAL 14)
        message(STATUS "Set homebrew's llvm")
        set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm@7/bin/clang++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/opt/llvm@7/include")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L/usr/local/opt/llvm@7/lib")
    endif()

    # -DCMAKE_BUILD_TYPE=Debug
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address")
    # -DCMAKE_BUILD_TYPE=Release
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mtune=native -march=native")
else()
    if(WIN32 AND MSVC)
        link_directories(${BOOST_LIBRARYDIR})

        find_package(yaml-cpp REQUIRED)
        include_directories(${YAML_CPP_INCLUDE_DIR})

        find_package(OpenSSL REQUIRED)
        include_directories(${OPENSSL_INCLUDE_DIR})

        if(ENABLE_STATIC)
            string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
            set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
            string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL}")
            set(CMAKE_CXX_FLAGS_MINSIZEREL ${CMAKE_CXX_FLAGS_MINSIZEREL})
            string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELEASE  "${CMAKE_CXX_FLAGS_RELEASE}")
            set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})
            string(REPLACE "/MD" "/MT" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
            set(CMAKE_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})

            string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
            set(CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
            string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL}")
            set(CMAKE_C_FLAGS_MINSIZEREL ${CMAKE_C_FLAGS_MINSIZEREL})
            string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
            set(CMAKE_C_FLAGS_RELEASE ${CMAKE_C_FLAGS_RELEASE})
            string(REPLACE "/MD" "/MT" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
            set(CMAKE_C_FLAGS_RELWITHDEBINFO ${CMAKE_C_FLAGS_RELWITHDEBINFO})
        endif()
    elseif(${CMAKE_BUILD_TYPE} MATCHES Debug) # -DCMAKE_BUILD_TYPE=Debug
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address")
    elseif(${CMAKE_BUILD_TYPE} MATCHES Release) # -DCMAKE_BUILD_TYPE=Release
        set(STATIC_LINK_FLAG "-static")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3") # -mtune=powerpc64le -mcpu=powerpc64le
    endif()
endif()


add_executable(${PROJECT_NAME} main.cpp)
if(UNIX)
    target_link_libraries(${PROJECT_NAME}
            "${STATIC_LINK_FLAG}"
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            "ssl"
            "crypto"
            "dl"
            "yaml-cpp")
elseif(WIN32)
    target_link_libraries(${PROJECT_NAME}
            "${STATIC_LINK_FLAG}"
            ${Boost_FILESYSTEM_LIBRARY}
            ${Boost_SYSTEM_LIBRARY}
            ${OPENSSL_LIBRARIES}
            "yaml-cpp"
            "bcrypt"
            "ws2_32")
endif()

install(TARGETS poac DESTINATION bin)
