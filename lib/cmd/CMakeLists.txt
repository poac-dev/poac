add_library(poac_cmd
    build.cc
)

target_compile_features(poac_cmd PUBLIC cxx_std_20)
target_include_directories(poac_cmd PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
#    ${Boost_INCLUDE_DIRS}
    #    ${OPENSSL_INCLUDE_DIR}
)
target_link_libraries(poac_cmd PUBLIC
    ${STATIC_LINK_FLAG}

    ${POAC_HPP_LIBS}
    structopt::structopt
    toml11::toml11

    poac_core
    poac_util
)

add_library(poac::cmd ALIAS poac_cmd)
