set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/create")

set(TEST_NAMES
    create-binary
)

createTests("${TEST_NAMES}" Boost::ut)
