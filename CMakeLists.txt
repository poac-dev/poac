cmake_minimum_required(VERSION 3.9)
project(poac)

add_definitions( -std=c++14 -Wall )

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      OFF)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(SOURCE_FILES libcpp/poac-init.cpp)

set(ENV{POAC_ROOT} ~/Documents/project/poacpm/poac)

find_package(Boost 1.66.0 COMPONENTS filesystem)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIR})
    add_executable(poac ${SOURCE_FILES})
    target_link_libraries(poac ${Boost_LIBRARIES})
endif ()
