include_guard(GLOBAL)

if (APPLE)
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -mtune=native -march=native")
else ()
    set(STATIC_LINK_FLAG "-static") # ref: https://stackoverflow.com/a/3801032
    set(CMAKE_CXX_FLAGS_RELEASE "-O3") # -mtune=powerpc64le -mcpu=powerpc64le
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
endif ()
if (LINUX)
    set(Boost_USE_STATIC_LIBS ON)
endif ()
