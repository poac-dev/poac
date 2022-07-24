set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/util")

set(TEST_NAMES
    cfg
    lev_distance
    meta
    misc
    net
    pretty
    sha256
    shell
    validator
    verbosity
)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cc)

    if (POAC_ENABLE_COVERAGE)
        target_compile_options(${TEST_NAME} PRIVATE -g -O0 --coverage)
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    else ()
        target_compile_options(${TEST_NAME} PRIVATE -g -O0)
    endif ()

    target_link_libraries(${TEST_NAME} PRIVATE Boost::ut "poac_util_${TEST_NAME}")

    add_test(
        NAME "${TEST_NAME_PREFIX}/${TEST_NAME}"
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endforeach ()

add_subdirectory(semver)
add_subdirectory(termcolor2)
