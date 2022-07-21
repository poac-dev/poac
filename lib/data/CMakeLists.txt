add_library(poac_data
    lockfile.cc
)

target_compile_features(poac_data PUBLIC cxx_std_20)
target_compile_definitions(poac_data PUBLIC TOML11_NO_ERROR_PREFIX)
target_include_directories(poac_data PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
)
target_link_libraries(poac_data PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
    toml11::toml11
    poac_core_resolver
)
