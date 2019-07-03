# Reference: https://github.com/opencv/opencv/blob/e329c84d5e7cfa5965b5dbbe7aa94d377ede23cf/CMakeLists.txt#L9-L15
# Disable in-source builds to prevent source tree corruption.
if(" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif()


cmake_minimum_required(VERSION 3.0)
project(poac)

# Preprocessor definitions.
add_definitions(-DPOAC_PROJECT_ROOT="${CMAKE_SOURCE_DIR}")
add_definitions(-DPOAC_VERSION="0.2.1")

# Compiler options.
if(CYGWIN OR MINGW)
    add_definitions(-D_GNU_SOURCE -DBOOST_ASIO_HAS_STD_STRING_VIEW -Wa,-mbig-obj)
elseif(UNIX)
    add_definitions("-fdiagnostics-color -Wall -Wextra") # -Werror
    if(NOT APPLE)
        add_definitions(-D_GNU_SOURCE -DBOOST_ASIO_HAS_STD_STRING_VIEW)
    endif()
elseif(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /DBOOST_ASIO_HAS_STD_STRING_VIEW /bigobj /constexpr:steps500000")
endif()

# Check if C++17 can use.
if(MSVC)
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

# Find boost libraries.
#set(Boost_DEBUG on)
set(Boost_NO_BOOST_CMAKE ON)
find_package(Boost REQUIRED COMPONENTS system filesystem)
if(NOT ${Boost_FOUND})
    message(FATAL_ERROR "Could not find boost.")
endif()
include_directories(${Boost_INCLUDE_DIRS})
message(STATUS "Boost include directory is ... ${Boost_INCLUDE_DIRS}")
link_directories(${Boost_LIBRARY_DIRS})
message(STATUS "Boost library directories are ... ${Boost_LIBRARY_DIRS}")
# If boost version greater than 1.69.0 and cmake version less then 3.12, occur error.
if(${Boost_VERSION} GREATER 106900)
    if(${CMAKE_VERSION} VERSION_LESS 3.12)
        message(FATAL_ERROR "If cmake version less than 3.12, doesn't use Boost::system style.")
    endif()
    set(BOOST_LINK_LIBS Boost::system Boost::filesystem)
else()
    set(BOOST_LINK_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY})
endif()

# Find a yaml-cpp library.
if(NOT CYGWIN)
    if(WIN32)
        find_package(yaml-cpp REQUIRED)
    else()
        find_package(PkgConfig REQUIRED)
        pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
    endif()
    if(NOT ${YAML_CPP_FOUND})
        message(FATAL_ERROR "Could not find yaml-cpp.")
    endif()
    include_directories(${YAML_CPP_INCLUDE_DIR})
    message(STATUS "yaml-cpp include directory is ... ${YAML_CPP_INCLUDE_DIR}")
    link_directories(${YAML_CPP_LIBRARY_DIRS})
    message(STATUS "yaml-cpp library directories are ... ${YAML_CPP_LIBRARY_DIRS}")
    message(STATUS "yaml-cpp libraries are ... ${YAML_CPP_LIBRARIES}")
else()
    # In cygwin, could not find yaml-cpp:
    #   Package 'yaml-cpp', required by 'virtual:world', not found
    set(YAML_CPP_LIBRARIES yaml-cpp)
    link_directories(/usr/local/lib)
endif()

# Find a openssl library.
if(APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif()
find_package(OpenSSL REQUIRED)
if(NOT ${OPENSSL_FOUND})
    message(FATAL_ERROR "Could not find openssl.")
endif()
include_directories(${OPENSSL_INCLUDE_DIR})
message(STATUS "OpenSSL include directory is ... ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries are ... ${OPENSSL_LIBRARIES}")

# Build configures.
if(APPLE)
    # If macOS version less than mojave, set compiler to homebrew llvm@7.
    execute_process(
            COMMAND bash -c "sw_vers | head -n 2 | tail -n 1 | cut -f 2 | cut -f 2 -d \".\""
            OUTPUT_VARIABLE OSX_VERSION
    )
    if(${OSX_VERSION} LESS 14)
        set(HOMEBREW_LLVM_PATH /usr/local/opt/llvm@7)
        if(NOT EXISTS ${HOMEBREW_LLVM_PATH})
            message(FATAL_ERROR "You need llvm@7 to build. Please execute `brew install llvm@7` to install it.")
        endif()
        message(STATUS "Set compiler to homebrew llvm@7.")
        set(CMAKE_CXX_COMPILER "${HOMEBREW_LLVM_PATH}/bin/clang++")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I${HOMEBREW_LLVM_PATH}/include")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${HOMEBREW_LLVM_PATH}/lib")
        unset(HOMEBREW_LLVM_PATH)
    endif()

    # -DCMAKE_BUILD_TYPE=Debug
    set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address")
    # -DCMAKE_BUILD_TYPE=Release
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -mtune=native -march=native")
else()
    if((MSVC OR MINGW) AND ENABLE_STATIC)
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
    # -DCMAKE_BUILD_TYPE=Debug
    elseif(${CMAKE_BUILD_TYPE} MATCHES Debug)
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fsanitize=address")
    # -DCMAKE_BUILD_TYPE=Release
    elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
        set(STATIC_LINK_FLAG "-static")
        set(CMAKE_CXX_FLAGS_RELEASE "-O3") # -mtune=powerpc64le -mcpu=powerpc64le
    endif()
endif()

add_executable(${PROJECT_NAME} src/main.cpp)
include_directories(${CMAKE_SOURCE_DIR}/include)

if(UNIX)
    set(SSL_RELATED_LIBS dl)
elseif(WIN32)
    set(SSL_RELATED_LIBS bcrypt ws2_32)
endif()
target_link_libraries(
        ${PROJECT_NAME}
        ${STATIC_LINK_FLAG}
        ${OPENSSL_LIBRARIES}
        ${SSL_RELATED_LIBS}
        ${BOOST_LINK_LIBS}
        ${YAML_CPP_LIBRARIES}
)
install(TARGETS poac DESTINATION bin)
