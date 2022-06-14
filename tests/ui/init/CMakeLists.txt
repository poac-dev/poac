set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/init")

set(TEST_NAMES
    overwrite
    simple-init
)

createTests("${TEST_NAMES}" Boost::ut)
