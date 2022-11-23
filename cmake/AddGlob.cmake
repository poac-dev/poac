include_guard(GLOBAL)

message(CHECK_START "Adding Glob")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
    Glob
    GIT_REPOSITORY https://github.com/p-ranav/glob.git
    GIT_TAG        v0.0.1
)
FetchContent_MakeAvailable(Glob)

# Disable clang-tidy
set_target_properties(
    Glob
    PROPERTIES
        CXX_CLANG_TIDY ""
)

list(APPEND POAC_DEPENDENCIES Glob)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
