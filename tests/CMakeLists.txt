set(TEST_NAMES
        cfg
        except
        misc
        name
        net
        path
        pretty
        project
        resolve
        sat
        shell
        standard
        types
)

get_property(POAC_COMPILE_FEATURES TARGET poac PROPERTY COMPILE_FEATURES)
get_property(POAC_COMPILE_DEFINITIONS TARGET poac PROPERTY COMPILE_DEFINITIONS)
get_property(POAC_COMPILE_OPTIONS TARGET poac PROPERTY COMPILE_OPTIONS)
get_property(POAC_INCLUDE_DIRECTORIES TARGET poac PROPERTY INCLUDE_DIRECTORIES)
get_property(POAC_LINK_DIRECTORIES TARGET poac PROPERTY LINK_DIRECTORIES)
get_property(POAC_LINK_LIBRARIES TARGET poac PROPERTY LINK_LIBRARIES)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cpp)
    target_compile_features(${TEST_NAME} PRIVATE ${POAC_COMPILE_FEATURES})
    target_compile_definitions(${TEST_NAME} PRIVATE ${POAC_COMPILE_DEFINITIONS})
    target_compile_options(${TEST_NAME} PRIVATE ${POAC_COMPILE_OPTIONS})
    if (CMAKE_BUILD_TYPE STREQUAL Coverage)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    endif()
    target_include_directories(${TEST_NAME} PRIVATE ${POAC_INCLUDE_DIRECTORIES})
    if (${CMAKE_VERSION} VERSION_LESS 3.13)
        link_directories(${POAC_LINK_DIRECTORIES})
    else ()
        target_link_directories(${TEST_NAME} PRIVATE ${POAC_LINK_DIRECTORIES})
    endif()
    target_link_libraries(${TEST_NAME} ${POAC_LINK_LIBRARIES})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME} WORKING_DIRECTORY ${PROJECT_BINARY_DIR})
endforeach ()

add_subdirectory(semver)
add_subdirectory(termcolor2)
add_subdirectory(util)
