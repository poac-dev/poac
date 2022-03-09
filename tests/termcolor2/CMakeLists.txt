set(TEST_NAMES
#        gnu_literals
        literals
        to_color
)

get_property(POAC_INCLUDE_DIRECTORIES TARGET poac PROPERTY INCLUDE_DIRECTORIES)
get_property(POAC_LINK_DIRECTORIES TARGET poac PROPERTY LINK_DIRECTORIES)
get_property(POAC_LINK_LIBRARIES TARGET poac PROPERTY LINK_LIBRARIES)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_20)
    target_compile_options(${TEST_NAME} PRIVATE ${POAC_COMPILE_OPTIONS} -Wno-gnu-string-literal-operator-template)
    if (POAC_ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif()
    target_include_directories(${TEST_NAME} PRIVATE ${POAC_INCLUDE_DIRECTORIES})
    target_link_directories(${TEST_NAME} PRIVATE ${POAC_LINK_DIRECTORIES})
    target_link_libraries(${TEST_NAME} ${POAC_LINK_LIBRARIES} ${POAC_TEST_DEPENDENCIES})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endforeach ()
