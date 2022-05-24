#add_library(poac_util
#        lockfile.cc
#        )
#get_property(POAC_COMPILE_FEATURES TARGET poac PROPERTY COMPILE_FEATURES)
#get_property(POAC_COMPILE_DEFINITIONS TARGET poac PROPERTY COMPILE_DEFINITIONS)
#get_property(POAC_INCLUDE_DIRECTORIES TARGET poac PROPERTY INCLUDE_DIRECTORIES)
#get_property(POAC_LINK_DIRECTORIES TARGET poac PROPERTY LINK_DIRECTORIES)
#
#target_compile_features(poac_data PRIVATE ${POAC_COMPILE_FEATURES})
#target_compile_definitions(poac_data PRIVATE ${POAC_COMPILE_DEFINITIONS})
#target_include_directories(poac_data PRIVATE ${POAC_INCLUDE_DIRECTORIES})
#target_link_directories(poac_data PRIVATE ${POAC_LINK_DIRECTORIES})
#target_link_libraries(poac_data PRIVATE ${POAC_DEPENDENCIES})
#
#add_library(poac::data ALIAS poac_data)
#list(APPEND POAC_DEPENDENCIES poac::data)

add_subdirectory(semver)
