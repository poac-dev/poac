set(TEST_NAMES
        char_traits
        comparison
        concat
        gnu_literals
        io
        literals
        min
        string
        to_color
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_11)
    target_compile_options(${TEST_NAME} PRIVATE -g -O0 -Wno-gnu-string-literal-operator-template)
    if (CMAKE_BUILD_TYPE STREQUAL Coverage)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif()
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endforeach ()
