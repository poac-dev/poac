set(TEST_NAMES
        cfg
        meta
        misc
        net
        ninja_syntax
        pretty
        resolve
        sat
        shell
        validator
)

# Erase the compile options of Boost::ut because GCC provides many warnings.
# ref: https://github.com/boost-ext/ut/blob/67b136c0267a54248d05fcba63905ff5e6abf0b7/CMakeLists.txt#L83-L115
set_property(TARGET ut PROPERTY COMPILE_OPTIONS)

get_property(POAC_COMPILE_FEATURES TARGET poac PROPERTY COMPILE_FEATURES)
get_property(POAC_COMPILE_DEFINITIONS TARGET poac PROPERTY COMPILE_DEFINITIONS)
get_property(POAC_INCLUDE_DIRECTORIES TARGET poac PROPERTY INCLUDE_DIRECTORIES)
get_property(POAC_LINK_DIRECTORIES TARGET poac PROPERTY LINK_DIRECTORIES)
get_property(POAC_LINK_LIBRARIES TARGET poac PROPERTY LINK_LIBRARIES)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_compile_features(${TEST_NAME} PRIVATE ${POAC_COMPILE_FEATURES})
    target_include_directories(${TEST_NAME} PRIVATE ${POAC_INCLUDE_DIRECTORIES})
    if (TEST_NAME STREQUAL "cfg")  # cfg does not need dependencies of Poac.
        target_link_libraries(${TEST_NAME} PRIVATE ${POAC_TEST_DEPENDENCIES})
    else ()
        target_compile_definitions(${TEST_NAME} PRIVATE ${POAC_COMPILE_DEFINITIONS})
        target_link_directories(${TEST_NAME} PRIVATE ${POAC_LINK_DIRECTORIES})
        target_link_libraries(${TEST_NAME} PRIVATE ${POAC_LINK_LIBRARIES} ${POAC_TEST_DEPENDENCIES})
    endif ()

    target_compile_options(${TEST_NAME} PRIVATE -g -O0)
    if (POAC_ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif ()

    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endforeach ()

add_subdirectory(semver)
add_subdirectory(termcolor2)
