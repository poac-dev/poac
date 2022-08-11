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
    # ref: https://stackoverflow.com/a/39256013
    SET(CMAKE_AR  "gcc-ar")
    SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_CXX_ARCHIVE_FINISH true)
    enable_ipo()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (APPLE)
        # For nix, ref: https://github.com/NixOS/nixpkgs/issues/166205
        set(STATIC_LINK_FLAG "-lc++abi")
    endif ()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    enable_ipo()
else ()
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    enable_ipo()
endif ()

if (UNIX AND NOT APPLE)
    set(Boost_USE_STATIC_LIBS ON)
endif ()
