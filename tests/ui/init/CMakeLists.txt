set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/init")

set(TEST_NAMES
    overwrite
    simple-case
)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cc)
    target_compile_definitions(${TEST_NAME} PRIVATE POAC_EXECUTABLE="${POAC_EXECUTABLE}")
    if (POAC_ENABLE_COVERAGE)
        target_compile_options(${TEST_NAME} PRIVATE -g -O0 --coverage)
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    else ()
        target_compile_options(${TEST_NAME} PRIVATE -g -O0)
    endif ()
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_HOME_DIRECTORY}/include
    )
    target_link_libraries(${TEST_NAME} PRIVATE
        Boost::ut
#        mitama-cpp-result::mitama-cpp-result
    )
    add_test(
        NAME "${TEST_NAME_PREFIX}/${TEST_NAME}"
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endforeach ()
