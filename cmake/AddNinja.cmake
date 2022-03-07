include_guard(GLOBAL)

message(CHECK_START "Adding ninja")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        ninja
        GIT_REPOSITORY https://github.com/ninja-build/ninja
        GIT_TAG        v1.10.2
)
FetchContent_MakeAvailable(ninja)

file(COPY ${ninja_SOURCE_DIR}/src DESTINATION ${ninja_BINARY_DIR}/include)
file(RENAME ${ninja_BINARY_DIR}/include/src ${ninja_BINARY_DIR}/include/ninja RESULT POAC_DEV_NULL)
target_include_directories(${PROJECT_NAME} PRIVATE ${ninja_BINARY_DIR}/include)
list(APPEND POAC_DEPENDENCIES libninja)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
