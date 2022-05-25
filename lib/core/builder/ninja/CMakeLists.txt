add_library(poac_core_builder_ninja
    build.cc
    log.cc
    manifest.cc
    status_printer.cc
    syntax.cc
)

target_compile_features(poac_core_builder_ninja PUBLIC cxx_std_20)
target_include_directories(poac_core_builder_ninja PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
    ${Boost_INCLUDE_DIRS}
    ${NINJA_INCLUDE_DIR}
)
target_link_libraries(poac_core_builder_ninja PUBLIC
    ${STATIC_LINK_FLAG}
    ${POAC_HPP_LIBS}
    ${NINJA_LIBRARIES}
    Boost::regex
    toml11::toml11
    poac_util
    poac_core_builder_compiler_cxx
    poac_core_resolver
)

add_library(poac::core::builder::ninja ALIAS poac_core_builder_ninja)
