set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/core")

set(TEST_NAMES
    validator
)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cc)
    if (POAC_ENABLE_COVERAGE)
        target_compile_options(${TEST_NAME} PRIVATE -g -O0 --coverage)
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    else ()
        target_compile_options(${TEST_NAME} PRIVATE -g -O0)
    endif ()
    target_link_libraries(${TEST_NAME} PRIVATE
        Boost::ut
        ${POAC_HPP_LIBS}
        toml11::toml11
        poac::core
    )
    add_test(
        NAME "${TEST_NAME_PREFIX}/${TEST_NAME}"
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endforeach ()

add_subdirectory(builder)
add_subdirectory(resolver)
