set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/resolver")

set(TEST_NAMES
    resolve
    sat
)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cc)
    target_compile_options(${TEST_NAME} PRIVATE -g -O0)
    if (POAC_ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif ()
    target_link_libraries(${TEST_NAME} PRIVATE
        Boost::ut
        poac::core::resolver
    )
    add_test(
        NAME "${TEST_NAME_PREFIX}/${TEST_NAME}"
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endforeach ()
