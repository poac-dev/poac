set(test_name "subcmd_init_test")
add_executable(${test_name} ${CMAKE_CURRENT_LIST_DIR}/test.cpp)
add_test(NAME ${test_name}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMAND ${test_name})
