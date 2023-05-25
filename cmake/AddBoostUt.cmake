include_guard(GLOBAL)

message(CHECK_START "Adding Boost::ut")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

set(FETCHCONTENT_UPDATES_DISCONNECTED_UT ON)
FetchContent_Declare(
    ut
    GIT_REPOSITORY https://github.com/boost-ext/ut
    GIT_TAG        v1.1.9
    PATCH_COMMAND  git apply --ignore-whitespace "${CMAKE_CURRENT_LIST_DIR}/BoostUt.patch"
)

FetchContent_MakeAvailable(ut)

# target: Boost::ut
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
