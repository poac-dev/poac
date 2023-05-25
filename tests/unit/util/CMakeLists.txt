include_guard(GLOBAL)

set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/util")

set(TEST_NAMES
    cfg
    levDistance
    meta
#    misc
#    net
#    pretty
#    sha256
    shell
#    validator
#    verbosity
)

foreach (TEST_NAME ${TEST_NAMES})
    set(TARGET_NAME "poac_test_util_${TEST_NAME}")
    add_executable(${TARGET_NAME} "${TEST_NAME_PREFIX}/${TEST_NAME}.cc")

    if (POAC_ENABLE_COVERAGE)
        target_compile_options(${TARGET_NAME} PRIVATE -g -O0 --coverage)
        target_link_options(${TARGET_NAME} PRIVATE --coverage)
    else ()
        target_compile_options(${TARGET_NAME} PRIVATE -g -O0)
    endif ()

    target_link_libraries(${TARGET_NAME} PRIVATE Boost::ut "poac_util_${TEST_NAME}")

    add_test(
        NAME ${TARGET_NAME}
        COMMAND ${TARGET_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endforeach ()

#add_subdirectory(semver)
#add_subdirectory(termcolor2)
