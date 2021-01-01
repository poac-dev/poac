macro (add_poac_options)
    target_compile_options(${PROJECT_NAME} PRIVATE ${ARGV})
endmacro ()

macro (add_poac_definitions)
    target_compile_definitions(${PROJECT_NAME} PRIVATE ${ARGV})
endmacro ()
