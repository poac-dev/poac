include_guard(GLOBAL)

message(CHECK_START "Adding Boost")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(Boost_DEBUG ${VERBOSE})
if (DEFINED BOOST_ROOT)
    set(BOOST_LIBRARYDIR "${BOOST_ROOT}/lib")
endif()

find_package(Boost 1.70.0 REQUIRED COMPONENTS system regex)
if (Boost_FOUND)
    message(CHECK_PASS "added")
    message(STATUS "Boost include directories are ... ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost library directories are ... ${Boost_LIBRARY_DIRS}")
    message(STATUS "Boost libraries are ... ${Boost_LIBRARIES}")

    target_include_directories(poac PRIVATE ${Boost_INCLUDE_DIRS})
    target_link_directories(poac PRIVATE ${Boost_LIBRARY_DIRS})
    list(APPEND POAC_DEPENDENCIES ${Boost_LIBRARIES})

    if (POAC_CLANG_TIDY)
        # Disable clang-tidy
        set_target_properties(
            ${Boost_LIBRARIES}
            PROPERTIES
            CXX_CLANG_TIDY ""
        )
    endif ()
else ()
    message(CHECK_FAIL "not found")
    list(APPEND missingDependencies boost)
endif ()

list(POP_BACK CMAKE_MESSAGE_INDENT)
