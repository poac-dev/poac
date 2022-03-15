#ifndef POAC_CORE_BUILDER_COMPILER_CXX_CLANG_HPP
#define POAC_CORE_BUILDER_COMPILER_CXX_CLANG_HPP

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

namespace poac::core::builder::compiler::cxx::clang {
    namespace anyhow = mitama::anyhow;

    inline const util::cfg::compiler compiler = util::cfg::compiler::clang;

    [[nodiscard]] anyhow::result<semver::Version>
    get_compiler_version(const std::string& compiler_command) {
        const auto res = util::shell::cmd(compiler_command + " --version").exec();
        if (res.has_value()) {
            // `clang version 12.0.0 (...)`
            std::string version;
            for (std::size_t i = 14; i < res.value().size(); ++i) {
                if (res->operator[](i) == ' ') { // read until space
                    break;
                }
                version += res->operator[](i);
            }
            return mitama::success(semver::Version{version});
        }
        return anyhow::failure<error::Error::FailedToGetCompilerVersion>(
            error::to_string(compiler)
        );
    }

    // thanks to: https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/Clang.cmake
    [[nodiscard]] anyhow::result<std::string>
    get_std_flag(const std::string& compiler_command, const std::int64_t& cpp, const bool use_gnu_extension) {
        const semver::Version version = MITAMA_TRY(get_compiler_version(compiler_command));
        const std::string specifier = use_gnu_extension ? "gnu" : "c";
        switch (cpp) {
            case 98:
                if (version > "2.1") {
                    return mitama::success(fmt::format("-std={}++98", specifier));
                }
                break;
            case 11:
                if (version > "3.1") {
                    return mitama::success(fmt::format("-std={}++11", specifier));
                } else if (version > "2.1") {
                    return mitama::success(fmt::format("-std={}++0x", specifier));
                }
                break;
            case 14:
                if (version >= "3.5") {
                    return mitama::success(fmt::format("-std={}++14", specifier));
                } else if (version >= "3.4") {
                    return mitama::success(fmt::format("-std={}++1y", specifier));
                }
                break;
            case 17:
                if (version >= "5.0") {
                    return mitama::success(fmt::format("-std={}++17", specifier));
                } else if (version >= "3.5") {
                    return mitama::success(fmt::format("-std={}++1z", specifier));
                }
                break;
            case 20:
                if (version >= "11.0") {
                    return mitama::success(fmt::format("-std={}++20", specifier));
                } else if (version >= "5.0") {
                    return mitama::success(fmt::format("-std={}++2a", specifier));
                }
                break;
        }
        return anyhow::failure<error::Error::UnsupportedLangVersion>(
            error::to_string(compiler), version.get_full(), lang::lang_t::cxx, cpp
        );
    }
} // end namespace

#endif // !POAC_CORE_BUILDER_COMPILER_CXX_CLANG_HPP
