set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/fmt")

set(TEST_NAMES
    empty-name
    invalid-manifest
    no-clang-format
    no-manifest
    no-targets
)

createTests("${TEST_NAMES}"
    Boost::ut
    mitama-cpp-result::mitama-cpp-result
)
