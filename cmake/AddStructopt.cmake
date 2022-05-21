include_guard(GLOBAL)

message(CHECK_START "Adding structopt")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        structopt
        GIT_REPOSITORY https://github.com/p-ranav/structopt.git
        GIT_TAG        v0.1.3
)
FetchContent_MakeAvailable(structopt)
list(APPEND POAC_DEPENDENCIES structopt::structopt)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
