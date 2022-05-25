add_library(poac_cmd
    build.cc
    create.cc
    fmt.cc
    init.cc
    lint.cc
    login.cc
    publish.cc
    run.cc
    search.cc
)

target_compile_features(poac_cmd PUBLIC cxx_std_20)
target_include_directories(poac_cmd PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include
    ${Boost_INCLUDE_DIRS}
    ${LIBGIT2_INCLUDE_DIR}
)
target_link_libraries(poac_cmd PUBLIC
    ${STATIC_LINK_FLAG}

    ${POAC_HPP_LIBS}
    structopt::structopt
    toml11::toml11
    git2
    git2-cpp::git2-cpp
    Glob

    poac_core
    poac_util
)

add_library(poac::cmd ALIAS poac_cmd)
