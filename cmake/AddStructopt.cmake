include_guard(GLOBAL)

message(CHECK_START "Adding structopt")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        structopt
        GIT_REPOSITORY https://github.com/p-ranav/structopt.git
        GIT_TAG        422c2e293702047b52e420651742f73581ad4677
)
FetchContent_MakeAvailable(structopt)
list(APPEND POAC_DEPENDENCIES structopt::structopt)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
