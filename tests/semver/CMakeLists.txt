set(TEST_NAMES
        interval
        lexer
#        parser
        token
        version
)

get_property(POAC_LINK_DIRECTORIES TARGET poac PROPERTY LINK_DIRECTORIES)
get_property(POAC_LINK_LIBRARIES TARGET poac PROPERTY LINK_LIBRARIES)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_17)
    set(CXX_COMPILE_FLAGS "-g -O0")  # to erase the INTERFACE of Boost::ut options: https://github.com/boost-ext/ut/blob/67b136c0267a54248d05fcba63905ff5e6abf0b7/CMakeLists.txt#L84
    if (POAC_ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif ()
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/include ${Boost_INCLUDE_DIRS})
    target_link_directories(${TEST_NAME} PRIVATE ${POAC_LINK_DIRECTORIES})
    target_link_libraries(${TEST_NAME} PRIVATE ${POAC_LINK_LIBRARIES} ${POAC_TEST_DEPENDENCIES})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endforeach ()
