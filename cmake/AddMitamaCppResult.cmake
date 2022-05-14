include_guard(GLOBAL)

message(CHECK_START "Adding mitama-cpp-result")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        mitama-cpp-result
        GIT_REPOSITORY https://github.com/LoliGothick/mitama-cpp-result.git
        GIT_TAG        80cfbce0382a27262c15339a22a6f35246cac65e
)
FetchContent_MakeAvailable(mitama-cpp-result)
list(APPEND POAC_DEPENDENCIES mitama-cpp-result::mitama-cpp-result)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
