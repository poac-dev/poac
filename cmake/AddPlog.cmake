include_guard(GLOBAL)

message(CHECK_START "Adding Plog")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        plog
        GIT_REPOSITORY https://github.com/SergiusTheBest/plog.git
        GIT_TAG        1.1.5
)
set(CMAKE_PROJECT_plog_INCLUDE_BEFORE "${CMAKE_SOURCE_DIR}/cmake/PlogPoliciesFix.cmake")
FetchContent_MakeAvailable(plog)
list(APPEND POAC_DEPENDENCIES plog::plog)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
