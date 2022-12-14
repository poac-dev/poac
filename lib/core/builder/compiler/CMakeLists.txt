add_poac_target(poac_core_builder_compiler
    SOURCES
        error.cc
        lang.cc
    LIBRARIES
        ${POAC_HPP_LIBS}
        poac_util_cfg
)

add_subdirectory(cxx)
