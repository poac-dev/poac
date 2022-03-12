include_guard(GLOBAL)

message(CHECK_START "Adding Toml11")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        toml11
        GIT_REPOSITORY https://github.com/ToruNiina/toml11.git
        GIT_TAG        v3.7.1
)

set(CMAKE_PROJECT_toml11_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/CMP0077PolicyFix.cmake")
set(toml11_BUILD_TEST OFF)
FetchContent_MakeAvailable(toml11)

list(APPEND POAC_DEPENDENCIES toml11::toml11)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
