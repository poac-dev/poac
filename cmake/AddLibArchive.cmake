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
    message(CHECK_PASS "added")
    message(STATUS "LibArchive include directory is ... ${LibArchive_INCLUDE_DIR}")
    message(STATUS "LibArchive library is ... ${LibArchive_LIBRARIES}")

    target_include_directories(${PROJECT_NAME} PRIVATE ${LibArchive_INCLUDE_DIR})
    list(APPEND POAC_DEPENDENCIES ${LibArchive_LIBRARIES})
else ()
    message(CHECK_FAIL "not found")
    list(APPEND missingDependencies libarchive)
endif ()

list(POP_BACK CMAKE_MESSAGE_INDENT)
