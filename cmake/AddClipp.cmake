include_guard(GLOBAL)

message(CHECK_START "Adding Clipp")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        clipp
        GIT_REPOSITORY https://github.com/muellan/clipp.git
        GIT_TAG        2c32b2f1f7cc530b1ec1f62c92f698643bb368db
)
FetchContent_MakeAvailable(clipp)
list(APPEND POAC_DEPENDENCIES clipp::clipp)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
