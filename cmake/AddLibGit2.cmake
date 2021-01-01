include_guard(GLOBAL)

message(CHECK_START "Adding libgit2")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

if (DEFINED LIBGIT2_DIR)
    target_include_directories(${PROJECT_NAME} PRIVATE ${LIBGIT2_DIR}/include)
    target_link_directories(${PROJECT_NAME} PRIVATE ${LIBGIT2_DIR}/lib)
endif ()
list(APPEND POAC_DEPENDENCIES git2)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
