include_guard(GLOBAL)
include(cmake/Helpers.cmake)

message(CHECK_START "Adding Poac dependencies")
list(APPEND CMAKE_MESSAGE_INDENT "  ")
unset(missingDependencies)

include(FetchContent)
list_dir_items(DEPENDENCIES ${CMAKE_SOURCE_DIR}/cmake)
list(FILTER DEPENDENCIES INCLUDE REGEX "Add.*cmake")  # Add files that match with the regex
foreach (DEP IN LISTS DEPENDENCIES)
    include(${CMAKE_SOURCE_DIR}/cmake/${DEP})
endforeach()

list(POP_BACK CMAKE_MESSAGE_INDENT)
if(missingDependencies)
    message(CHECK_FAIL "missing dependencies: ${missingDependencies}")
    message(FATAL_ERROR "missing dependencies found")
else()
    message(CHECK_PASS "all dependencies are added")
endif()

message(STATUS "dependencies are ... ${POAC_DEPENDENCIES}")
