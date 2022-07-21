set(TEST_NAME_PREFIX "${TEST_NAME_PREFIX}/util")

set(TEST_NAMES
    cfg
    lev_distance
    meta
    misc
    net
    pretty
    sha256
    shell
    verbosity
)

foreach (TEST_NAME ${TEST_NAMES})
    add_executable(${TEST_NAME} ${TEST_NAME}.cc)
    target_compile_features(${TEST_NAME} PRIVATE cxx_std_20)

    if (POAC_ENABLE_COVERAGE)
        target_compile_options(${TEST_NAME} PRIVATE -g -O0 --coverage)
        target_link_options(${TEST_NAME} PRIVATE --coverage)
    else ()
        target_compile_options(${TEST_NAME} PRIVATE -g -O0)
    endif ()

    if (${TEST_NAME} STREQUAL "meta" OR ${TEST_NAME} STREQUAL "shell" OR ${TEST_NAME} STREQUAL "verbosity")
        target_include_directories(${TEST_NAME} PUBLIC
            ${CMAKE_HOME_DIRECTORY}/include
            ${Boost_INCLUDE_DIRS}
        )
        target_link_libraries(${TEST_NAME} PRIVATE
            Boost::ut
            fmt::fmt
            mitama-cpp-result::mitama-cpp-result
            spdlog::spdlog
        )
    else ()
        target_link_libraries(${TEST_NAME} PRIVATE Boost::ut "poac_util_${TEST_NAME}")
        if (${TEST_NAME} STREQUAL "net")
            target_link_libraries(${TEST_NAME} PRIVATE poac_util_pretty poac_util_misc) # FIXME: why needed?
        endif ()
    endif ()

    add_test(
        NAME "${TEST_NAME_PREFIX}/${TEST_NAME}"
        COMMAND ${TEST_NAME}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    )
endforeach ()

add_subdirectory(semver)
add_subdirectory(termcolor2)
