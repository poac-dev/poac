include_guard(GLOBAL)

message(CHECK_START "Adding mitama-cpp-result")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        mitama-cpp-result
        GIT_REPOSITORY https://github.com/LoliGothick/mitama-cpp-result.git
        GIT_TAG        v9.3.0
)
FetchContent_MakeAvailable(mitama-cpp-result)
# target: mitama-cpp-result::mitama-cpp-result
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
