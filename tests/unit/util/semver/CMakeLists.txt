include_guard(GLOBAL)

set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/semver")

set(TEST_NAMES
#    interval
#    lexer
#    parser
#    token
#    version
)

foreach (TEST_NAME ${TEST_NAMES})
    set(TARGET_NAME "test_semver_${TEST_NAME}")
    add_executable(${TARGET_NAME} "${TEST_NAME_PREFIX}/${TEST_NAME}.cc")

    if (POAC_ENABLE_COVERAGE)
        target_compile_options(${TARGET_NAME} PRIVATE -g -O0 --coverage)
        target_link_options(${TARGET_NAME} PRIVATE --coverage)
    else ()
        target_compile_options(${TARGET_NAME} PRIVATE -g -O0)
    endif ()

    target_link_libraries(${TARGET_NAME} PRIVATE
        Boost::ut
        semver
    )

    add_test(
        NAME ${TARGET_NAME}
        COMMAND ${TARGET_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endforeach ()
