include_guard(GLOBAL)

message(CHECK_START "Adding OpenSSL")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

if (APPLE)
    set(OPENSSL_ROOT_DIR "${POAC_HOMEBREW_ROOT_PATH}/openssl")
endif ()
if (DEFINED OPENSSL_ROOT_DIR)
    set(OPENSSL_LIBRARIES ${OPENSSL_ROOT_DIR}/lib)
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    message(CHECK_PASS "added")
    if (UNIX AND NOT APPLE)
        list(APPEND OPENSSL_LIBRARIES ssl crypto dl)
    elseif (APPLE)
        list(APPEND OPENSSL_LIBRARIES dl)
    elseif (WIN32)
        list(APPEND OPENSSL_LIBRARIES bcrypt ws2_32)
    endif ()
    message(STATUS "OpenSSL include directory is ... ${OPENSSL_INCLUDE_DIR}")
    message(STATUS "OpenSSL libraries are ... ${OPENSSL_LIBRARIES}")

    # TODO: Move this to the root CMakeLists.txt
    target_include_directories(poac_util_sha256 PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(poac_util_sha256 PRIVATE ${OPENSSL_LIBRARIES})
    target_include_directories(poac_util_net PRIVATE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(poac_util_net PRIVATE ${OPENSSL_LIBRARIES})

    target_include_directories(poac PRIVATE ${OPENSSL_INCLUDE_DIR})
else ()
    message(CHECK_FAIL "not found")
    list(APPEND missingDependencies openssl)

    if (APPLE)
        message(FATAL_ERROR "OpenSSL not found. Please install it by `brew install openssl@3`")
    endif ()
endif ()

list(POP_BACK CMAKE_MESSAGE_INDENT)
