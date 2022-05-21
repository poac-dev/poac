include_guard(GLOBAL)

message(CHECK_START "Adding Spdlog")
list(APPEND CMAKE_MESSAGE_INDENT "  ")

FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.10.0
)

set(SPDLOG_FMT_EXTERNAL ON)
if (CMAKE_BUILD_TYPE STREQUAL Release) # -DCMAKE_BUILD_TYPE=Release
    set(SPDLOG_BUILD_SHARED OFF)
else ()
    set(SPDLOG_BUILD_SHARED ON)
endif ()
FetchContent_MakeAvailable(spdlog)

list(APPEND POAC_DEPENDENCIES spdlog::spdlog)
message(CHECK_PASS "added")

list(POP_BACK CMAKE_MESSAGE_INDENT)
