# ref: https://stackoverflow.com/a/7788165
macro (list_dir_items result curdir)
    file(GLOB items RELATIVE ${curdir} ${curdir}/*)
    set(${result} ${items})
endmacro ()

macro (get_homebrew_prefix_path)
    execute_process(
        COMMAND brew --prefix
        OUTPUT_VARIABLE POAC_HOMEBREW_PREFIX_PATH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endmacro ()

if (APPLE)
    get_homebrew_prefix_path()
    set(POAC_HOMEBREW_ROOT_PATH "${POAC_HOMEBREW_PREFIX_PATH}/opt")
endif()

# If no sources are provided, then only an interface target will be added.
function (add_poac_target target_name)
    cmake_parse_arguments(VARS "" "CXX" "SOURCES;INCLUDES;LIBRARIES;DEFINES" ${ARGN})

    if (NOT DEFINED VARS_CXX)
        set(VARS_CXX_STD cxx_std_20)
    else ()
        set(VARS_CXX_STD cxx_std_${VARS_CXX})
    endif ()

    add_library(${target_name} INTERFACE) # prepare an interface target for dependents
    target_compile_features(${target_name} INTERFACE ${VARS_CXX_STD})
    target_compile_definitions(${target_name} INTERFACE ${VARS_DEFINES})
    target_include_directories(${target_name} INTERFACE ${VARS_INCLUDES})

    list(LENGTH VARS_SOURCES SOURCE_COUNT)
    if (${SOURCE_COUNT} GREATER 0)
        add_library(${target_name}_obj OBJECT ${VARS_SOURCES})
        target_compile_features(${target_name}_obj PUBLIC ${VARS_CXX_STD})
        target_compile_definitions(${target_name}_obj PUBLIC ${VARS_DEFINES})

        target_include_directories(${target_name}_obj PUBLIC ${VARS_INCLUDES})
        target_link_libraries(${target_name}_obj PUBLIC ${STATIC_LINK_FLAG} ${VARS_LIBRARIES})

        target_link_libraries(${target_name} INTERFACE
            ${STATIC_LINK_FLAG}
            ${VARS_LIBRARIES}
            ${target_name}_obj
            $<TARGET_OBJECTS:${target_name}_obj>
        )
    else ()
        target_link_libraries(${target_name} INTERFACE
            ${STATIC_LINK_FLAG}
            ${VARS_LIBRARIES}
        )
    endif ()
endfunction ()
