include_guard(GLOBAL)

function(enable_ipo)
    include(CheckIPOSupported)
    check_ipo_supported(
        RESULT ipo_supported
        OUTPUT ipo_check_output
        LANGUAGES CXX
    )
    if (ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    else ()
        message(WARNING "IPO / LTO is not supported: ${ipo_check_output}")
    endif ()
endfunction()

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    # ref: https://stackoverflow.com/a/3801032
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -flto -mtune=native")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # ref: https://gist.github.com/ken-matsui/f6c736ea9623cc15e0a1e8912cae5718
    # I'm not sure why, but the `-static` flag fails on GCC with release builds
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    enable_ipo()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(STATIC_LINK_FLAG "-static")
#    if (${CMAKE_CXX_COMPILER} MATCHES "-[0-9]+$")
#        string(REGEX REPLACE [[.*clang\+\+(-[0-9]+)$]] [[lld\1]] LINKER ${CMAKE_CXX_COMPILER})
#        set(STATIC_LINK_FLAG "-static -fuse-ld=${LINKER}")
#    else ()
#        set(STATIC_LINK_FLAG "-static -fuse-ld=lld")
#    endif ()
    enable_ipo()
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    set(STATIC_LINK_FLAG "-static")
    enable_ipo()
endif ()

if (UNIX AND NOT APPLE)
    set(Boost_USE_STATIC_LIBS ON)
endif ()
