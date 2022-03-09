include_guard(GLOBAL)

message(CHECK_START "Adding boost.ut")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        ut
        GIT_REPOSITORY https://github.com/boost-ext/ut
        GIT_TAG        v1.1.9
)

FetchContent_MakeAvailable(ut)

list(APPEND POAC_DEPENDENCIES ut)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
