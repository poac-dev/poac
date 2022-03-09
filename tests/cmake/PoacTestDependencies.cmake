include_guard(GLOBAL)

message(CHECK_START "Adding Poac test dependencies")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
unset(missingDependencies)

include(FetchContent)

file(GLOB items RELATIVE ${CMAKE_SOURCE_DIR}/tests/cmake ${CMAKE_SOURCE_DIR}/tests/cmake/*)
set(TEST_DEPENDENCIES ${items})
list(FILTER TEST_DEPENDENCIES INCLUDE REGEX "Add.*cmake")  # Add files that match with the regex
foreach (DEP IN LISTS TEST_DEPENDENCIES)
    include(${CMAKE_SOURCE_DIR}/tests/cmake/${DEP})
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
if(missingDependencies)
    message(CHECK_FAIL "missing test dependencies: ${missingDependencies}")
    message(FATAL_ERROR "missing test dependencies found")
else()
    message(CHECK_PASS "all test dependencies are added")
endif()

message(STATUS "test dependencies are ... ${POAC_TEST_DEPENDENCIES}")
