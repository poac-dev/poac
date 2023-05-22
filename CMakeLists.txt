# Reference: https://github.com/opencv/opencv/blob/e329c84d5e7cfa5965b5dbbe7aa94d377ede23cf/CMakeLists.txt#L9-L15
# Disable in-source builds to prevent source tree corruption.
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif ()

cmake_minimum_required(VERSION 3.26)
project(poac
    VERSION 0.6.0
    LANGUAGES CXX
)
message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "Poac Version: ${CMAKE_PROJECT_VERSION}")
message(STATUS "Compiler Path: ${CMAKE_CXX_COMPILER}")

# C++20 Modules
set(CMAKE_CXX_STANDARD 20) # turn on the dynamic depends for ninja
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "2182bf5c-ef0d-489a-91da-49dbc3090d2a")
set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP 1)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # cf. https://gitlab.kitware.com/cmake/cmake/-/blob/master/.gitlab/ci/cxx_modules_rules_gcc.cmake?ref_type=heads
    string(CONCAT CMAKE_EXPERIMENTAL_CXX_SCANDEP_SOURCE
        "<CMAKE_CXX_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -E -x c++ <SOURCE>"
        " -MT <DYNDEP_FILE> -MD -MF <DEP_FILE>"
        " -fmodules-ts -fdep-file=<DYNDEP_FILE> -fdep-output=<OBJECT> -fdep-format=trtbd"
        " -o <PREPROCESSED_SOURCE>"
    )
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FORMAT "gcc")
    set(CMAKE_EXPERIMENTAL_CXX_MODULE_MAP_FLAG
        "-fmodules-ts -fmodule-mapper=<MODULE_MAP_FILE> -fdep-format=trtbd -x c++"
    )
endif()
if (APPLE)
    # export CXX=/opt/homebrew/opt/llvm/bin/clang++
endif ()

# Set the default build type to Debug
if (CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif ()

option(VERBOSE "Use verbose output" OFF) # -DVERBOSE=ON
option(CPM_USE_LOCAL_PACKAGES "Try to use local packages" ON)
option(POAC_BUILD_TESTING "Enable build tests" ON)

include(CMakeDependentOption)
cmake_dependent_option(
    POAC_CLANG_TIDY "Enable clang-tidy when Debug mode" ON
    "CMAKE_BUILD_TYPE STREQUAL Debug" OFF
)
cmake_dependent_option(
    POAC_ENABLE_COVERAGE "Enable coverage" OFF
    "POAC_BUILD_TESTING" OFF
)

# Option checks
if ((NOT POAC_BUILD_TESTING) AND POAC_ENABLE_COVERAGE)
    message(FATAL_ERROR "POAC_ENABLE_COVERAGE cannot be enabled without POAC_BUILD_TESTING being on")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ${VERBOSE})
set(CMAKE_CXX_EXTENSIONS OFF) # without compiler extensions like gnu++11
if (CPM_USE_LOCAL_PACKAGES)
    set(CPM_SUPPORTED_LIBS LibArchive Libgit2 fmt spdlog)
    message(STATUS "CPM_USE_LOCAL_PACKAGES is ON; `${CPM_SUPPORTED_LIBS}` on local will be used if found.")
endif ()

include(src/mod.cmake)

add_executable(poac src/main.cc)
target_compile_features(poac PUBLIC cxx_std_20)
target_link_libraries(poac PRIVATE
    # Direct dependencies

    poac_cmd
    poac_util_result_macros
    poac_util_result
    poac_util_levDistance

    structopt::structopt
    termcolor2_literals_extra
    mitama-cpp-result::mitama-cpp-result
    toml11::toml11
)

include(cmake/PoacConfig.cmake)
target_link_libraries(poac PRIVATE ${STATIC_LINK_FLAG})

include(cmake/Helpers.cmake)
include(cmake/PoacDependencies.cmake)
if (NOT APPLE AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND POAC_DEPENDENCIES "-lstdc++fs")
endif ()
target_link_libraries(poac PRIVATE
    # Indirect dependencies
    # TODO: should be derived from other deps?
    ${LIBGIT2_LIBRARY}
    ${NINJA_LIBRARIES}
)

set(CONFIG_VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/poac-config-version.cmake)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CONFIG_VERSION_FILE} COMPATIBILITY AnyNewerVersion
)

include(GNUInstallDirs)
install(TARGETS poac
    EXPORT poac-config
)
install(EXPORT poac-config
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/poac
    NAMESPACE poac::
)
install(FILES ${CONFIG_VERSION_FILE}
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/poac
)
install(TARGETS poac
    DESTINATION bin
)

include(CTest)
if (POAC_BUILD_TESTING AND ${CMAKE_SOURCE_DIR} STREQUAL ${PROJECT_SOURCE_DIR})
    enable_testing()
    add_subdirectory(tests)
endif ()
