add_poac_target(poac_core_builder_compiler_lang
    SOURCES
        error.cc
        lang.cc
    INCLUDES ${POAC_HPP_INCLUDES}
    LIBRARIES
        ${POAC_HPP_LIBS}
        poac_util_cfg
)
