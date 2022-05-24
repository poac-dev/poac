add_subdirectory(semver)

add_library(poac_util
    archive.cc
    cfg.cc
    lev_distance.cc
    misc.cc
    pretty.cc
    sha256.cc
    shell.cc
)
#get_property(POAC_COMPILE_FEATURES TARGET poac PROPERTY COMPILE_FEATURES)
#get_property(POAC_COMPILE_DEFINITIONS TARGET poac PROPERTY COMPILE_DEFINITIONS)
#get_property(POAC_INCLUDE_DIRECTORIES TARGET poac PROPERTY INCLUDE_DIRECTORIES)
#get_property(POAC_LINK_DIRECTORIES TARGET poac PROPERTY LINK_DIRECTORIES)

target_compile_features(poac_util PUBLIC cxx_std_20)
target_include_directories(poac_util PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
    ${Boost_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)
target_link_libraries(poac_util PUBLIC
    archive
    fmt::fmt
    mitama-cpp-result::mitama-cpp-result
    ${OPENSSL_LIBRARIES}
    spdlog::spdlog
)
#target_compile_features(poac_data PRIVATE ${POAC_COMPILE_FEATURES})
#target_compile_definitions(poac_data PRIVATE ${POAC_COMPILE_DEFINITIONS})
#target_include_directories(poac_data PRIVATE ${POAC_INCLUDE_DIRECTORIES})
#target_link_directories(poac_data PRIVATE ${POAC_LINK_DIRECTORIES})
#target_link_libraries(poac_data PRIVATE ${POAC_DEPENDENCIES})

add_library(poac::util ALIAS poac_util)
#list(APPEND POAC_DEPENDENCIES poac::util)
