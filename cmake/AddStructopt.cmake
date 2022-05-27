include_guard(GLOBAL)

message(CHECK_START "Adding structopt")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        structopt
        GIT_REPOSITORY https://github.com/p-ranav/structopt.git
        GIT_TAG        34621d927317a7a81c46ae11fd678c0223e152a5
)
FetchContent_MakeAvailable(structopt)
list(APPEND POAC_DEPENDENCIES structopt::structopt)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
