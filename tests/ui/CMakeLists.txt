set(TEST_NAME_PREFIX ui)
set(POAC_EXECUTABLE "${CMAKE_BINARY_DIR}/poac")

macro(createTests TEST_NAMES)
    foreach (TEST_NAME ${TEST_NAMES})
        add_executable(${TEST_NAME} ${TEST_NAME}.cc)
        target_compile_definitions(${TEST_NAME} PRIVATE POAC_EXECUTABLE="${POAC_EXECUTABLE}")
        if (POAC_ENABLE_COVERAGE)
            target_compile_options(${TEST_NAME} PRIVATE -g -O0 --coverage)
            target_link_options(${TEST_NAME} PRIVATE --coverage)
        else ()
            target_compile_options(${TEST_NAME} PRIVATE -g -O0)
        endif ()
        target_include_directories(${TEST_NAME} PRIVATE
            ${CMAKE_HOME_DIRECTORY}/include
        )
        target_link_libraries(${TEST_NAME} PRIVATE ${ARGN})
        add_test(
            NAME "${TEST_NAME_PREFIX}/${TEST_NAME}"
            COMMAND ${TEST_NAME}
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        )
    endforeach ()
endmacro()

add_subdirectory(create)
add_subdirectory(fmt)
add_subdirectory(init)
