include_guard(GLOBAL)

add_library(poac_cmd_build)
target_sources(poac_cmd_build
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/build.ixx
)
target_link_libraries(poac_cmd_build PRIVATE
    poac_config
    poac_core_builder_build
    poac_core_resolver_types
    poac_core_resolver
    poac_data_manifest

    poac_util_log
    poac_util_pretty
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_validator

    structopt::structopt
    toml11::toml11
    spdlog::spdlog
)

add_library(poac_cmd_clean)
target_sources(poac_cmd_clean
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/clean.ixx
)
target_link_libraries(poac_cmd_clean PRIVATE
    poac_config
    poac_cmd_build
    poac_data_manifest

    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_validator

    structopt::structopt
    spdlog::spdlog
)

add_library(poac_cmd_create)
target_sources(poac_cmd_create
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/create.ixx
)
target_link_libraries(poac_cmd_create PRIVATE
    poac_config
    poac_data_manifest
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_validator

    structopt::structopt
    spdlog::spdlog
    git2-cpp::git2-cpp
)

add_library(poac_cmd_fmt)
target_sources(poac_cmd_fmt
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/fmt.ixx
)
target_link_libraries(poac_cmd_fmt PRIVATE
    poac_config
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_shell
    poac_util_validator
    poac_util_verbosity

    structopt::structopt
    spdlog::spdlog
    Glob
    toml11::toml11
)

add_library(poac_cmd_graph)
target_sources(poac_cmd_graph
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/graph.ixx
)
target_link_libraries(poac_cmd_graph PRIVATE
    poac_config
    poac_cmd_build
    poac_core_resolver_types
    poac_core_resolver
    poac_data_manifest

    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_shell

    structopt::structopt
    spdlog::spdlog
    toml11::toml11
)

add_library(poac_cmd_init)
target_sources(poac_cmd_init
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/init.ixx
)
target_link_libraries(poac_cmd_init PRIVATE
    poac_config
    poac_cmd_create
    poac_data_manifest
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_validator

    structopt::structopt
    spdlog::spdlog
)

add_library(poac_cmd_lint)
target_sources(poac_cmd_lint
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/lint.ixx
)
target_link_libraries(poac_cmd_lint PRIVATE
    poac_config
    poac_cmd_fmt
    poac_data_manifest

    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_shell
    poac_util_validator
    poac_util_verbosity

    structopt::structopt
    spdlog::spdlog
    toml11::toml11
)

add_library(poac_cmd_login)
target_sources(poac_cmd_login
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/login.ixx
)
target_link_libraries(poac_cmd_login PRIVATE
    poac_config
    poac_util_format
    poac_util_log
    poac_util_net
    poac_util_result_macros
    poac_util_result
    poac_util_rustify

    structopt::structopt
    spdlog::spdlog
)

add_library(poac_cmd_publish)
target_sources(poac_cmd_publish
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/publish.ixx
)
target_link_libraries(poac_cmd_publish PRIVATE
    poac_config
    poac_cmd_build
    poac_cmd_login
    poac_data_manifest

    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_validator

    structopt::structopt
    spdlog::spdlog
    toml11::toml11
)

add_library(poac_cmd_run)
target_sources(poac_cmd_run
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/run.ixx
)
target_link_libraries(poac_cmd_run PRIVATE
    poac_cmd_build
    poac_data_manifest

    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_shell
    poac_util_validator

    structopt::structopt
    spdlog::spdlog
    toml11::toml11
)

add_library(poac_cmd_search)
target_sources(poac_cmd_search
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd/search.ixx
)
target_link_libraries(poac_cmd_search PRIVATE
    poac_util_format
    poac_util_log
    poac_util_net
    poac_util_pretty
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_verbosity

    structopt::structopt
    spdlog::spdlog
)
