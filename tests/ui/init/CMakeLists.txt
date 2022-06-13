set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/init")

set(TEST_NAMES
    overwrite
    simple-case
)

createTests("${TEST_NAMES}" Boost::ut)
