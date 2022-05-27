set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/fmt")

set(TEST_NAMES
    empty-name
    invalid-manifest
    no-clang-format
    no-manifest
    no-targets
)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cc)
    target_compile_definitions(${TEST_NAME} PRIVATE POAC_EXECUTABLE="${POAC_EXECUTABLE}")
    target_compile_options(${TEST_NAME} PRIVATE -g -O0)
    if (POAC_ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif ()
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_HOME_DIRECTORY}/include
    )
    target_link_libraries(${TEST_NAME} PRIVATE
        Boost::ut
        mitama-cpp-result::mitama-cpp-result
    )
    add_test(
        NAME "${TEST_NAME_PREFIX}/${TEST_NAME}"
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endforeach ()
