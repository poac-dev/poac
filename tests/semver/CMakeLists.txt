set(TEST_NAMES
        interval
        lexer
        parser
        token
        version
)

find_package(Boost REQUIRED COMPONENTS unit_test_framework)
get_property(poac_COMPILE_FEATURES TARGET poac PROPERTY COMPILE_FEATURES)
get_property(poac_COMPILE_DEFINITIONS TARGET poac PROPERTY COMPILE_DEFINITIONS)
get_property(poac_COMPILE_OPTIONS TARGET poac PROPERTY COMPILE_OPTIONS)
get_property(poac_INCLUDE_DIRECTORIES TARGET poac PROPERTY INCLUDE_DIRECTORIES)
get_property(poac_LINK_DIRECTORIES TARGET poac PROPERTY LINK_DIRECTORIES)
get_property(poac_LINK_LIBRARIES TARGET poac PROPERTY LINK_LIBRARIES)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_compile_features(${TEST_NAME} PRIVATE ${poac_COMPILE_FEATURES})
    target_compile_definitions(${TEST_NAME} PRIVATE ${poac_COMPILE_DEFINITIONS})
    target_compile_options(${TEST_NAME} PRIVATE ${poac_COMPILE_OPTIONS})
    if (CMAKE_BUILD_TYPE STREQUAL Coverage)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif()
    target_include_directories(${TEST_NAME} PRIVATE
            ${PROJECT_SOURCE_DIR}/include ${poac_INCLUDE_DIRECTORIES} ${Boost_INCLUDE_DIRS})
    target_link_directories(${TEST_NAME} PRIVATE ${poac_LINK_DIRECTORIES})
    target_link_libraries(${TEST_NAME} ${poac_LINK_LIBRARIES} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endforeach ()
