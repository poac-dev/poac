include_guard(GLOBAL)

message(CHECK_START "Adding mitama-cpp-result")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        mitama-cpp-result
        GIT_REPOSITORY https://github.com/LoliGothick/mitama-cpp-result.git
        GIT_TAG        1cfe393e7d02f66cf0303e6df77099e3a4b20d32
)
FetchContent_MakeAvailable(mitama-cpp-result)
list(APPEND POAC_DEPENDENCIES mitama-cpp-result::mitama-cpp-result)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
