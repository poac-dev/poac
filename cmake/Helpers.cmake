macro (add_poac_options)
    target_compile_options(${PROJECT_NAME} PRIVATE ${ARGV})
endmacro ()

macro (add_poac_definitions)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${ARGV})
endmacro ()

# ref: https://stackoverflow.com/a/7788165
macro (list_dir_items result curdir)
    file(GLOB items RELATIVE ${curdir} ${curdir}/*)
    set(${result} ${items})
endmacro ()

macro (get_arch)
    execute_process(
        COMMAND uname -m
        OUTPUT_VARIABLE POAC_ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro ()

if (APPLE)
    get_arch()
    if (POAC_ARCHITECTURE STREQUAL "arm64")
        set(POAC_HOMEBREW_ROOT_PATH /opt/homebrew/opt)
    else ()
        set(POAC_HOMEBREW_ROOT_PATH /usr/local/opt)
    endif()
endif()
