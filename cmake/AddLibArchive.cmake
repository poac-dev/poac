include_guard(GLOBAL)

message(CHECK_START "Adding LibArchive")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

if (APPLE)
    set(LibArchive_INCLUDE_DIR "/usr/local/opt/libarchive/include")
elseif (DEFINED LIBARCHIVE_DIR)
    set(LibArchive_INCLUDE_DIR "${LIBARCHIVE_DIR}/include")
endif()

find_package(LibArchive)
if (LibArchive_FOUND)
    if (STATIC_LINK_FLAG STREQUAL "-static" AND NOT APPLE AND ${LibArchive_LIBRARIES} MATCHES ".*\.so$")
        unset(LibArchive_LIBRARIES)
        unset(LibArchive_LIBRARY CACHE)
        find_library(LibArchive_LIBRARY
            NAMES libarchive.a)
        mark_as_advanced(LibArchive_LIBRARY)
        set(LibArchive_LIBRARIES ${LibArchive_LIBRARY})
        set_target_properties(LibArchive::LibArchive PROPERTIES
            IMPORTED_LOCATION "${LibArchive_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${LibArchive_INCLUDE_DIR}")
    endif ()
    message(CHECK_PASS "added")
    message(STATUS "LibArchive include directory is ... ${LibArchive_INCLUDE_DIR}")
    message(STATUS "LibArchive library is ... ${LibArchive_LIBRARIES}")

    target_include_directories(${PROJECT_NAME} PRIVATE ${LibArchive_INCLUDE_DIR})
    list(APPEND POAC_DEPENDENCIES ${LibArchive_LIBRARIES})
    if (STATIC_LINK_FLAG STREQUAL "-static" AND NOT APPLE)
        list(APPEND POAC_DEPENDENCIES acl)
        list(APPEND POAC_DEPENDENCIES z)
    endif ()
else ()
    message(CHECK_FAIL "not found")
    list(APPEND missingDependencies libarchive)
endif ()

list(POP_BACK CMAKE_MESSAGE_INDENT)
