#ifndef POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP
#define POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP

// std
#include <cstddef> // std::size_t

// external
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>

// internal
#include <poac/core/builder/compiler/error.hpp>
#include <poac/util/semver/semver.hpp>
#include <poac/util/cfg.hpp>
#include <poac/util/shell.hpp>

namespace poac::core::builder::compiler::cxx::apple_clang {
    namespace anyhow = mitama::anyhow;

    inline const util::cfg::compiler compiler = util::cfg::compiler::apple_clang;

    [[nodiscard]] anyhow::result<semver::Version>
    get_compiler_version(const std::string& compiler_command) {
        const auto res = util::shell::cmd(compiler_command + " --version").dump_stderr().exec();
        if (res.has_value()) {
            // `Apple clang version 13.0.0 (...)`
            std::string version;
            for (std::size_t i = 20; i < res.value().size(); ++i) {
                if (res->operator[](i) == ' ') { // read until space
                    break;
                }
                version += res->operator[](i);
            }
            return mitama::success(semver::parse(version));
        }
        return anyhow::failure<error::Error::FailedToGetCompilerVersion>(
            error::to_string(compiler)
        );
    }

    // thanks to: https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/AppleClang-CXX.cmake
    [[nodiscard]] anyhow::result<std::string>
    get_std_flag(const std::string& compiler_command, const std::int64_t& cpp, const bool use_gnu_extension) {
        const semver::Version version = MITAMA_TRY(get_compiler_version(compiler_command));
        const std::string specifier = use_gnu_extension ? "gnu" : "c";
        switch (cpp) {
            case 98:
                if (version >= "4.0.0") {
                    return mitama::success(fmt::format("-std={}++98", specifier));
                }
                break;
            case 11:
                if (version >= "4.0.0") {
                    return mitama::success(fmt::format("-std={}++11", specifier));
                }
                break;
            case 14:
                if (version >= "6.1.0") {
                    return mitama::success(fmt::format("-std={}++14", specifier));
                } else if (version >= "5.1.0") {
                    // AppleClang 5.0 knows this flag, but does not set a __cplusplus macro greater than 201103L
                    return mitama::success(fmt::format("-std={}++1y", specifier));
                }
                break;
            case 17:
                if (version >= "10.0.0") {
                    return mitama::success(fmt::format("-std={}++17", specifier));
                } else if (version >= "6.1.0") {
                    return mitama::success(fmt::format("-std={}++1z", specifier));
                }
                break;
            case 20:
                if (version >= "13.0.0") {
                    return mitama::success(fmt::format("-std={}++20", specifier));
                } else if (version >= "10.0.0") {
                    return mitama::success(fmt::format("-std={}++2a", specifier));
                }
                break;
        }
        return anyhow::failure<error::Error::UnsupportedLangVersion>(
            error::to_string(compiler), version.get_full(), lang::lang_t::cxx, cpp
        );
    }
} // end namespace

#endif // !POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP
