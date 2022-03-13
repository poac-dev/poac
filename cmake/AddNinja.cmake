include_guard(GLOBAL)

message(CHECK_START "Adding ninja")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        ninja
        GIT_REPOSITORY https://github.com/ninja-build/ninja
        GIT_TAG        f0fd305a5772fe06be9627ddcdcf2950449900b9
)

set(BUILD_TESTING OFF)
FetchContent_MakeAvailable(ninja)

file(MAKE_DIRECTORY ${ninja_BINARY_DIR}/include)
file(CREATE_LINK ${ninja_SOURCE_DIR}/src ${ninja_BINARY_DIR}/include/ninja SYMBOLIC)
target_include_directories(${PROJECT_NAME} PRIVATE ${ninja_BINARY_DIR}/include)
list(APPEND POAC_DEPENDENCIES libninja libninja-re2c)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
