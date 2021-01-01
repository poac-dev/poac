# Reference: https://github.com/opencv/opencv/blob/e329c84d5e7cfa5965b5dbbe7aa94d377ede23cf/CMakeLists.txt#L9-L15
# Disable in-source builds to prevent source tree corruption.
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
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

include(cmake/CheckCompilerVersion.cmake)

add_executable(${PROJECT_NAME} src/main.cpp)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

include(cmake/PoacConfig.cmake)
include(cmake/AddPoacDependencies.cmake)
target_link_libraries(
        ${PROJECT_NAME}
        ${STATIC_LINK_FLAG}
        ${POAC_DEPENDENCIES}
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
