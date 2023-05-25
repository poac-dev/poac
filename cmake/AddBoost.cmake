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

    set(DEPENDENTS
        poac_util_rustify
        poac_util_cfg
        poac_util_misc
        poac_util_pretty
        poac_util_meta
        poac_util_archive
        poac_util_registry_conan_v1_manifest
        poac_core_resolver_sat
        poac_core_resolver_registry
        poac_cmd_lint
        poac_util_net
        poac_util_registry_conan_v1_resolver
        poac_core_resolver
        poac_core_builder_syntax
        poac_core_builder_manifest
        poac_core_builder_build
        poac_core_resolver_types
        poac_cmd_graph
        poac_cmd_search

        poac_test_util_cfg
        poac_test_util_meta
    )
    foreach (dependent ${DEPENDENTS})
        target_include_directories(${dependent} PRIVATE ${Boost_INCLUDE_DIRS})
    endforeach ()
    unset(DEPENDENTS)

    # mitama-cpp-result depends on Boost internally
    target_include_directories(poac_util_result_macros INTERFACE ${Boost_INCLUDE_DIRS})
else ()
    message(CHECK_FAIL "not found")
    list(APPEND missingDependencies boost)
endif ()

list(POP_BACK CMAKE_MESSAGE_INDENT)
