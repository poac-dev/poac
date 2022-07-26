# Reference: https://github.com/opencv/opencv/blob/e329c84d5e7cfa5965b5dbbe7aa94d377ede23cf/CMakeLists.txt#L9-L15
# Disable in-source builds to prevent source tree corruption.
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
    message(FATAL_ERROR "
FATAL: In-source builds are not allowed.
       You should create a separate directory for build files.
")
endif ()

cmake_minimum_required(VERSION 3.21)
project(poac
    VERSION 0.4.0
    LANGUAGES CXX
)
message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "Poac Version: ${CMAKE_PROJECT_VERSION}")

include(GNUInstallDirs)

option(VERBOSE "Use verbose output" OFF) # -DVERBOSE=ON
option(POAC_BUILD_TESTING "Do not build tests by default" OFF)
option(POAC_ENABLE_COVERAGE "Enable coverage" OFF)

# Option checks
if ((NOT POAC_BUILD_TESTING) AND POAC_ENABLE_COVERAGE)
    message(FATAL_ERROR "POAC_ENABLE_COVERAGE cannot be enabled without POAC_BUILD_TESTING being on")
endif ()

set(CMAKE_VERBOSE_MAKEFILE ${VERBOSE})
set(CMAKE_CXX_EXTENSIONS OFF) # without compiler extensions like gnu++11
if (CPM_USE_LOCAL_PACKAGES)
    set(CPM_SUPPORTED_LIBS LibArchive Libgit2 fmt spdlog)
    message(STATUS "CPM_USE_LOCAL_PACKAGES is set as ON; `${CPM_SUPPORTED_LIBS}` on local will be used if found.")
endif ()

add_executable(poac src/main.cc)
target_compile_features(poac PUBLIC cxx_std_20)
target_include_directories(poac PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(cmake/PoacConfig.cmake)
include(cmake/PoacDependencies.cmake)
if (NOT APPLE AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    list(APPEND POAC_DEPENDENCIES "-lstdc++fs")
endif ()

include(cmake/Helpers.cmake)
add_subdirectory(lib)
target_link_libraries(poac PRIVATE
    ${STATIC_LINK_FLAG}
    poac_cmd
    poac_util_lev_distance
    ${NINJA_LIBRARIES} # TODO: this should be propagated from poac_core_builder_ninja
)

set(CONFIG_VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/poac-config-version.cmake)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CONFIG_VERSION_FILE} COMPATIBILITY AnyNewerVersion
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)
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
