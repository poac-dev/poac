# Reference: https://github.com/opencv/opencv/blob/e329c84d5e7cfa5965b5dbbe7aa94d377ede23cf/CMakeLists.txt#L9-L15
# Disable in-source builds to prevent source tree corruption.
if (" ${CMAKE_SOURCE_DIR}" STREQUAL " ${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif ()

cmake_minimum_required(VERSION 3.17)
project(poac
        VERSION 0.2.1
        LANGUAGES CXX
)

include(GNUInstallDirs)
option(VERBOSE "Use verbose output" OFF) # -DVERBOSE=ON
set(CMAKE_VERBOSE_MAKEFILE ${VERBOSE})
set(CMAKE_CXX_EXTENSIONS OFF) # without compiler extensions like gnu++11

message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")

# Check required version
if (APPLE)
    execute_process(
        COMMAND bash -c "sw_vers | head -n 2 | tail -n 1 | cut -f 2"
        OUTPUT_VARIABLE MACOS_VERSION)
    if (${MACOS_VERSION} VERSION_LESS 10.15)
        message(FATAL_ERROR "Poac requires macOS Catalina (10.15) or later.")
    endif()
elseif (MSVC)
    # https://cmake.org/cmake/help/latest/variable/MSVC_TOOLSET_VERSION.html
    if (${MSVC_TOOLSET_VERSION} LESS 141)
        message(FATAL_ERROR "Poac requires Visual Studio 2017 or later.")
    endif()
else ()
    # https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8)
            message(FATAL_ERROR "Poac requires GCC 8 or later.")
        endif ()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    endif ()
endif ()

add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_definitions(${PROJECT_NAME} PRIVATE POAC_VERSION="${PROJECT_VERSION}")

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /DBOOST_ASIO_HAS_STD_STRING_VIEW /bigobj)
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # undefined reference to symbol 'pthread_condattr_setclock@@GLIBC_2.3.3'
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(${PROJECT_NAME} PRIVATE -fdiagnostics-color -Wall -Wextra -Werror)
    else ()
        target_compile_options(${PROJECT_NAME} PRIVATE -fdiagnostics-color -Wall -Wextra -Werror -pedantic-errors)
    endif ()
    if (APPLE)
        target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_BEAST_USE_STD_STRING_VIEW)
    else ()
        target_compile_definitions(${PROJECT_NAME} PRIVATE _GNU_SOURCE BOOST_ASIO_HAS_STD_STRING_VIEW)
    endif ()
    if (CYGWIN OR MINGW)
        target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Wa,-mbig-obj)
    endif ()
endif ()

if ((MSVC OR MINGW) AND ENABLE_STATIC)
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
elseif (CMAKE_BUILD_TYPE STREQUAL Debug) # -DCMAKE_BUILD_TYPE=Debug
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address")
elseif (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    if (APPLE)
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -mtune=native -march=native")
    else ()
        set(STATIC_LINK_FLAG "-static") # ref: https://stackoverflow.com/a/3801032
        set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto") # -mtune=powerpc64le -mcpu=powerpc64le
    endif ()
    if (LINUX)
        set(Boost_USE_STATIC_LIBS ON)
    endif ()
endif ()

if (DEFINED POAC_DEPS_DIR)
    set(BOOST_ROOT "${POAC_DEPS_DIR}/boost")
    set(LIBGIT2_DIR "${POAC_DEPS_DIR}/libgit2")
endif ()

set(Boost_DEBUG ${VERBOSE})
if (DEFINED BOOST_ROOT)
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
endif()
find_package(Boost 1.70.0 REQUIRED COMPONENTS system)
if (NOT ${Boost_FOUND})
    message(FATAL_ERROR "Could not find boost.")
endif ()
target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
message(STATUS "Boost include directory is ... ${Boost_INCLUDE_DIRS}")
target_link_directories(${PROJECT_NAME} PRIVATE ${Boost_LIBRARY_DIRS})
message(STATUS "Boost library directories are ... ${Boost_LIBRARY_DIRS}")
message(STATUS "Boost libraries are ... ${Boost_LIBRARIES}")

if (DEFINED LIBGIT2_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE ${LIBGIT2_DIR}/include)
    target_link_directories(${PROJECT_NAME} PRIVATE ${LIBGIT2_DIR}/lib)
endif ()

include(FetchContent)
FetchContent_Declare(
    toml11
    GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
    GIT_TAG        v3.6.0
)
set(toml11_BUILD_TEST OFF)
FetchContent_MakeAvailable(toml11)
list(APPEND POAC_DEPENDENCIES toml11::toml11)

FetchContent_Declare(
    clipp
    GIT_REPOSITORY https://github.com/muellan/clipp.git
    GIT_TAG        2c32b2f1f7cc530b1ec1f62c92f698643bb368db
)
FetchContent_MakeAvailable(clipp)
list(APPEND POAC_DEPENDENCIES clipp::clipp)

FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG        7.1.3
)
FetchContent_MakeAvailable(fmt)
list(APPEND POAC_DEPENDENCIES fmt::fmt)

FetchContent_Declare(
    plog
    GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
    GIT_TAG        1.1.5
)
set(CMAKE_PROJECT_plog_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/PlogPoliciesFix.cmake")
FetchContent_MakeAvailable(plog)
list(APPEND POAC_DEPENDENCIES plog::plog)

FetchContent_Declare(
    mitama-cpp-result
    GIT_REPOSITORY https://github.com/LoliGothick/mitama-cpp-result.git
    GIT_TAG        37e80b09e8dcf10ef4c2925de87a92cc750ecd90
)
FetchContent_MakeAvailable(mitama-cpp-result)
list(APPEND POAC_DEPENDENCIES mitama-cpp-result::mitama-cpp-result)

if (APPLE)
    set(LibArchive_INCLUDE_DIR "/usr/local/opt/libarchive/include")
elseif (DEFINED LIBARCHIVE_DIR)
    set(LibArchive_INCLUDE_DIR "${LIBARCHIVE_DIR}/include")
endif()
find_package(LibArchive)
if (NOT ${LibArchive_FOUND})
    message(FATAL_ERROR "Could not find LibArchive.")
endif ()
message(STATUS "LibArchive include directory is ... ${LibArchive_INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE ${LibArchive_INCLUDE_DIR})
message(STATUS "LibArchive library is ... ${LibArchive_LIBRARIES}")

if (APPLE)
    set(OPENSSL_ROOT_DIR /usr/local/opt/openssl)
endif ()
if (DEFINED OPENSSL_ROOT_DIR)
    set(OPENSSL_LIBRARIES ${OPENSSL_ROOT_DIR}/lib)
endif ()
find_package(OpenSSL REQUIRED)
if (NOT ${OPENSSL_FOUND})
    message(FATAL_ERROR "Could not find openssl.")
endif ()
target_include_directories(${PROJECT_NAME} PRIVATE ${OPENSSL_INCLUDE_DIR})
message(STATUS "OpenSSL include directory is ... ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libraries are ... ${OPENSSL_LIBRARIES}")

if (LINUX)
    set(SSL_RELATED_LIBS ssl crypto dl)
elseif (APPLE)
    set(SSL_RELATED_LIBS dl)
elseif (WIN32)
    set(SSL_RELATED_LIBS bcrypt ws2_32)
endif ()

message(STATUS "dependencies are ... ${POAC_DEPENDENCIES}")

target_link_libraries(
        ${PROJECT_NAME}
        ${STATIC_LINK_FLAG}
        ${OPENSSL_LIBRARIES}
        ${SSL_RELATED_LIBS}
        git2
        ${POAC_DEPENDENCIES}
        Boost::system
        ${LibArchive_LIBRARIES}
)

set(CONFIG_VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CONFIG_VERSION_FILE} COMPATIBILITY AnyNewerVersion
)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
)
install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}-config
)
install(EXPORT ${PROJECT_NAME}-config
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
        NAMESPACE ${PROJECT_NAME}::
)
install(FILES ${CONFIG_VERSION_FILE}
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}
)
install(TARGETS ${PROJECT_NAME}
        DESTINATION bin
)

option(BUILD_TESTING "Do not build tests by default" OFF) # -DBUILD_TESTING=ON
include(CTest)
if (BUILD_TESTING AND ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    enable_testing()
    add_subdirectory(tests)
endif ()
