find_package(Boost REQUIRED COMPONENTS unit_test_framework)
include_directories(${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIR})

file(GLOB_RECURSE TEST_SOURCES ${CMAKE_SOURCE_DIR}/test/*.cpp)

add_executable(unittest ${TEST_SOURCES})
target_link_libraries(unittest
        ${Boost_FILESYSTEM_LIBRARY}
        ${Boost_SYSTEM_LIBRARY}
        ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
        )
add_test(NAME poac_test
         WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
         COMMAND unittest)
