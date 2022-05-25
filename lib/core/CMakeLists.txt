add_library(poac_core
    resolver.cc
    validator.cc
)

target_compile_features(poac_core PUBLIC cxx_std_20)
target_include_directories(poac_core PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
    ${Boost_INCLUDE_DIRS}
#    ${OPENSSL_INCLUDE_DIR}
)
target_link_libraries(poac_core PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
    poac_core_resolver
    poac_data
    poac_util
    toml11::toml11
)

add_library(poac::core ALIAS poac_core)

add_subdirectory(builder)
add_subdirectory(resolver)
