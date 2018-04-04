cmake_minimum_required(VERSION 3.9)
project(poac)

add_definitions( -std=c++17 -Wall -Wextra )

set(Boost_USE_STATIC_LIBS         OFF)
set(Boost_USE_MULTITHREADED       OFF)
set(Boost_USE_STATIC_RUNTIME      OFF)

file(GLOB SOURCES libcpp/*)

find_package(Boost 1.66.0 COMPONENTS filesystem)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIR})
    add_executable(${PROJECT_NAME} ${SOURCES})
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif ()
