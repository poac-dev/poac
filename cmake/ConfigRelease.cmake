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
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    # ref: https://stackoverflow.com/a/39256013
    SET(CMAKE_AR  "gcc-ar")
    SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> qcs <TARGET> <LINK_FLAGS> <OBJECTS>")
    SET(CMAKE_CXX_ARCHIVE_FINISH true)
    enable_ipo()
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if (APPLE)
        # For nix, ref: https://github.com/NixOS/nixpkgs/issues/166205
        set(STATIC_FLAG " -lc++abi")
    endif ()

    if (${CMAKE_CXX_COMPILER} MATCHES "-[0-9]+$")
        string(REGEX REPLACE [[.*clang\+\+(-[0-9]+)$]] [[lld\1]] LINKER ${CMAKE_CXX_COMPILER})
        set(STATIC_LINK_FLAG "-fuse-ld=${LINKER}${STATIC_FLAG}")
    else ()
        find_program(lld_EXECUTABLE lld)
        if (lld_EXECUTABLE)
            set(STATIC_LINK_FLAG "-fuse-ld=lld${STATIC_FLAG}")
        else ()
            set(STATIC_LINK_FLAG "${STATIC_FLAG}") # use `ld`
        endif ()
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
