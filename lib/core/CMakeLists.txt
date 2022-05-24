#add_library(poac_core
#    validator.cc
#)
#get_property(POAC_COMPILE_FEATURES TARGET poac PROPERTY COMPILE_FEATURES)
#get_property(POAC_COMPILE_DEFINITIONS TARGET poac PROPERTY COMPILE_DEFINITIONS)
#get_property(POAC_INCLUDE_DIRECTORIES TARGET poac PROPERTY INCLUDE_DIRECTORIES)
#get_property(POAC_LINK_DIRECTORIES TARGET poac PROPERTY LINK_DIRECTORIES)
#
#target_compile_features(poac_core PRIVATE ${POAC_COMPILE_FEATURES})
#target_compile_definitions(poac_core PRIVATE ${POAC_COMPILE_DEFINITIONS})
#target_include_directories(poac_core PRIVATE ${POAC_INCLUDE_DIRECTORIES})
#target_link_directories(poac_core PRIVATE ${POAC_LINK_DIRECTORIES})
#target_link_libraries(poac_core PRIVATE ${POAC_DEPENDENCIES})
#
#add_library(poac::core ALIAS poac_core)
#list(APPEND POAC_DEPENDENCIES poac::core)

add_subdirectory(resolver)
