include_guard(GLOBAL)

include(src/util/mod.cmake)

add_library(poac_config)
target_sources(poac_config
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/config.ixx
)
target_link_libraries(poac_config PRIVATE
    poac_util_misc
    poac_util_rustify
)


add_library(poac_data_manifest)
target_sources(poac_data_manifest
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/data/manifest.ixx
)
target_link_libraries(poac_data_manifest PRIVATE
    poac_util_rustify

    toml11::toml11
)


add_library(poac_util_registry_conan_v1_manifest)
target_sources(poac_util_registry_conan_v1_manifest
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/registry/conan/v1/manifest.ixx
)
target_link_libraries(poac_util_registry_conan_v1_manifest PRIVATE
    poac_config
    poac_util_format
    poac_util_rustify
    poac_util_result
)


add_library(poac_util_validator)
target_sources(poac_util_validator
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/validator.ixx
)
target_link_libraries(poac_util_validator PRIVATE
    poac_config
    poac_data_manifest
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify

    toml11::toml11
    spdlog::spdlog
    semver
)


add_library(poac_core_resolver_sat)
target_sources(poac_core_resolver_sat
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/resolver/sat.ixx
)
target_link_libraries(poac_core_resolver_sat PRIVATE
    poac_util_format
    poac_util_log
    poac_util_result
    poac_util_rustify
)

add_library(poac_core_resolver_registry)
target_sources(poac_core_resolver_registry
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/resolver/registry.ixx
)
target_link_libraries(poac_core_resolver_registry PRIVATE
    poac_util_result
    poac_util_rustify
)


add_library(poac_core_builder_compiler_lang)
target_sources(poac_core_builder_compiler_lang
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/compiler/lang.ixx
)
target_link_libraries(poac_core_builder_compiler_lang PRIVATE
    poac_util_cfg
    poac_util_format
    poac_util_log
    poac_util_result
    poac_util_rustify

    fmt::fmt
)

add_library(poac_core_builder_compiler_error)
target_sources(poac_core_builder_compiler_error
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/compiler/error.ixx
)
target_link_libraries(poac_core_builder_compiler_error PRIVATE
    poac_core_builder_compiler_lang
    poac_util_cfg
    poac_util_format
    poac_util_log
    poac_util_result
    poac_util_rustify

    fmt::fmt
    semver_token
)

add_library(poac_core_builder_compiler_cxx_apple_clang)
target_sources(poac_core_builder_compiler_cxx_apple_clang
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/compiler/cxx/apple_clang.ixx
)
target_link_libraries(poac_core_builder_compiler_cxx_apple_clang PRIVATE
    poac_core_builder_compiler_lang
    poac_core_builder_compiler_error

    poac_util_cfg
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_shell

    semver
)
add_library(poac_core_builder_compiler_cxx_clang)
target_sources(poac_core_builder_compiler_cxx_clang
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/compiler/cxx/clang.ixx
)
target_link_libraries(poac_core_builder_compiler_cxx_clang PRIVATE
    poac_core_builder_compiler_lang
    poac_core_builder_compiler_error

    poac_util_cfg
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_shell

    semver
)
add_library(poac_core_builder_compiler_cxx_gcc)
target_sources(poac_core_builder_compiler_cxx_gcc
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/compiler/cxx/gcc.ixx
)
target_link_libraries(poac_core_builder_compiler_cxx_gcc PRIVATE
    poac_core_builder_compiler_lang
    poac_core_builder_compiler_error

    poac_util_cfg
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_shell

    semver
)
add_library(poac_core_builder_compiler_cxx_cxx)
target_sources(poac_core_builder_compiler_cxx_cxx
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/compiler/cxx/cxx.ixx
)
target_link_libraries(poac_core_builder_compiler_cxx_cxx PRIVATE
    poac_core_builder_compiler_cxx_apple_clang
    poac_core_builder_compiler_cxx_clang
    poac_core_builder_compiler_cxx_gcc
    poac_core_builder_compiler_error

    poac_util_misc
    poac_util_cfg
    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_shell
)

add_library(poac_core_builder_data)
target_sources(poac_core_builder_data
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/data.ixx
)
target_link_libraries(poac_core_builder_data PRIVATE
    poac_util_format
    poac_util_log
    poac_util_result
    poac_util_rustify

    spdlog::spdlog
)


add_library(poac_core_resolver_types)
target_sources(poac_core_resolver_types
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/resolver/types.ixx
)
target_link_libraries(poac_core_resolver_types PRIVATE
    poac_util_result
    poac_util_rustify
)


add_library(poac_util_net)
target_sources(poac_util_net
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/net.ixx
)
target_link_libraries(poac_util_net PRIVATE
    poac_core_resolver_types
    poac_util_format
    poac_util_log
    poac_util_meta
    poac_util_misc
    poac_util_pretty
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_verbosity

    spdlog::spdlog
)


add_library(poac_data_lockfile)
target_sources(poac_data_lockfile
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/data/lockfile.ixx
)
target_link_libraries(poac_data_lockfile PRIVATE
    poac_config
    poac_core_resolver_types
    poac_data_manifest
    poac_util_result_macros
    poac_util_result
    poac_util_rustify

    toml11::toml11
)


add_library(poac_util_registry_conan_v1_resolver)
target_sources(poac_util_registry_conan_v1_resolver
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/util/registry/conan/v1/resolver.ixx
)
target_link_libraries(poac_util_registry_conan_v1_resolver PRIVATE
    poac_config
    poac_core_resolver_types
    poac_data_lockfile

    poac_util_file
    poac_util_format
    poac_util_log
    poac_util_rustify
    poac_util_result_macros
    poac_util_result
    poac_util_shell
)


add_library(poac_core_resolver_resolve)
target_sources(poac_core_resolver_resolve
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/resolver/resolve.ixx
)
target_link_libraries(poac_core_resolver_resolve PRIVATE
    poac_core_resolver_sat
    poac_core_resolver_types

    poac_util_format
    poac_util_log
    poac_util_meta
    poac_util_net
    poac_util_registry_conan_v1_resolver
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_verbosity

    semver
)


add_library(poac_core_resolver)
target_sources(poac_core_resolver
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/resolver.ixx
)
target_link_libraries(poac_core_resolver PRIVATE
    poac_config

    poac_core_resolver_resolve
    poac_core_resolver_types
    poac_core_resolver_registry

    poac_data_manifest
    poac_data_lockfile

    poac_util_archive
    poac_util_file
    poac_util_format
    poac_util_log
    poac_util_meta
    poac_util_net
    poac_util_registry_conan_v1_resolver
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_sha256
    poac_util_shell

    toml11::toml11
    spdlog::spdlog
)


add_library(poac_core_builder_log)
target_sources(poac_core_builder_log
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/log.ixx
)
target_link_libraries(poac_core_builder_log PRIVATE
    poac_core_builder_data
    poac_util_format
    poac_util_log
    poac_util_result
    poac_util_rustify

    spdlog::spdlog
)

add_library(poac_core_builder_syntax)
target_sources(poac_core_builder_syntax
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/syntax.ixx
)
target_link_libraries(poac_core_builder_syntax PRIVATE
    poac_util_format
    poac_util_log
    poac_util_pretty
    poac_util_result
    poac_util_rustify
)

add_library(poac_core_builder_manifest)
target_sources(poac_core_builder_manifest
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/manifest.ixx
)
target_link_libraries(poac_core_builder_manifest PRIVATE
    poac_core_builder_data
    poac_core_builder_compiler_cxx_cxx
    poac_core_builder_syntax
    poac_core_resolver

    poac_util_cfg
    poac_util_format
    poac_util_log
    poac_util_registry_conan_v1_manifest
    poac_util_result_macros
    poac_util_result
    poac_util_rustify

    toml11::toml11
)

add_library(poac_core_builder_build)
target_sources(poac_core_builder_build
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/core/builder/build.ixx
)
target_link_libraries(poac_core_builder_build PRIVATE
    poac_config
    poac_core_builder_data
    poac_core_builder_log
    poac_core_builder_manifest
    poac_core_resolver_types

    poac_util_format
    poac_util_log
    poac_util_result_macros
    poac_util_result
    poac_util_rustify
    poac_util_verbosity

    spdlog::spdlog
    toml11::toml11
    termcolor2
    termcolor2_literals_extra
)


include(src/cmd/mod.cmake)

add_library(poac_cmd)
target_sources(poac_cmd
    PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES FILES
    src/cmd.ixx
)
target_link_libraries(poac_cmd PRIVATE
    poac_cmd_build
    poac_cmd_clean
    poac_cmd_create
    poac_cmd_fmt
    poac_cmd_graph
    poac_cmd_init
    poac_cmd_lint
    poac_cmd_login
    poac_cmd_publish
    poac_cmd_run
    poac_cmd_search
)
