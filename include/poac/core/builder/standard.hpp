#ifndef POAC_CORE_BUILDER_STANDARD_HPP
#define POAC_CORE_BUILDER_STANDARD_HPP

// std
#include <cstdint>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <string>
#include <string_view>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>

// internal
#include <poac/util/cfg.hpp>
#include <poac/util/semver/semver.hpp>
#include <poac/util/shell.hpp>
#include <poac/util/misc.hpp>

namespace poac::core::builder::standard {
    inline std::string version_prefix(const bool& enable_gnu) noexcept {
        return enable_gnu ? "-std=gnu++" : "-std=c++";
    }

    const std::string ANY = R"([\s\S]*)";

    [[nodiscard]] mitama::result<std::string, std::string>
    get_compiler_version(const std::string& compiler) {
        if (util::shell::has_command(compiler)) {
            if (const auto res = util::shell::cmd(compiler + " --version").stderr_to_stdout().exec()) {
                const std::regex SEARCH_VERSION("^" + ANY + "(" + semver::MAIN_VERSION + ")" + ANY + "$");
                std::smatch match;
                if (std::regex_match(*res, match, SEARCH_VERSION)) {
                    return mitama::success(match[1]);
                }
            }
        }
        return mitama::failure(
            fmt::format("Could not get {} version", compiler)
        );
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    apple_llvm_convert(const std::uint_fast8_t& cpp_version, const bool& enable_gnu) noexcept {
        // Apple LLVMは，コンパイラバージョンによる差異がないものとする．(半ば強制的にupdateされるため)
        if (cpp_version == 98) {
            return mitama::success(version_prefix(enable_gnu) + "98");
        } else if (cpp_version == 3 || cpp_version == 11) {
            return mitama::success(version_prefix(enable_gnu) + "11");
        } else if (cpp_version == 14) {
            return mitama::success(version_prefix(enable_gnu) + "14");
        } else if (cpp_version == 17) {
            return mitama::success(version_prefix(enable_gnu) + "17");
        } else if (cpp_version == 20) {
            return mitama::failure(
                "Currently, Apple LLVM does not support C++20.\n"
                "If this error is displayed in spite of C++20 is supported,\n"
                " please report the issue to https://github.com/poacpm/poac/issues."
            );
        } else {
            return mitama::failure(
                fmt::format("Unknown C++ version: {}", cpp_version)
            );
        }
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    gcc_convert(
        const std::uint_fast8_t& cpp_version,
        const std::string& compiler_version,
        const bool& enable_gnu
    ) noexcept {
        const semver::Version cv(compiler_version);

        if (cpp_version == 98) {
            return mitama::success(""); // unneeded version prefix
        } else if (cpp_version == 3 || cpp_version == 11) {
            if (cv < "4.3") {
                return mitama::failure("Invalid using C++11 less than gcc-4.3.0");
            } else if (cv >= "4.3" && cv < "4.7") {
                return mitama::success(version_prefix(enable_gnu) + "0x");
            } else {
                return mitama::success(version_prefix(enable_gnu) + "11");
            }
        } else if (cpp_version == 14) {
            if (cv < "4.8") {
                return mitama::failure("Invalid using C++14 less than gcc-4.8.0");
            } else if (cv >= "4.8" && cv < "4.9") {
                return mitama::success(version_prefix(enable_gnu) + "1y");
            } else {
                return mitama::success(version_prefix(enable_gnu) + "14");
            }
        } else if (cpp_version == 17) {
            if (cv < "5") {
                return mitama::failure("Invalid using C++17 less than gcc-5.0.0");
            } else {
                return mitama::success(version_prefix(enable_gnu) + "17");
            }
        } else if (cpp_version == 20) {
            if (cv < "8") {
                return mitama::failure("Invalid using C++20 less than gcc-8.0.0");
            } else {
                return mitama::success(version_prefix(enable_gnu) + "2a");
            }
        } else {
            return mitama::failure(
                fmt::format("Unknown C++ version: {}", cpp_version)
            );
        }
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    clang_convert(
        const std::uint_fast8_t& cpp_version,
        const std::string& compiler_version,
        const bool& enable_gnu
    ) {
        const semver::Version cv(compiler_version);

        if (cpp_version == 98) {
            return mitama::success(""); // unneeded version prefix
        } else if (cpp_version == 3 || cpp_version == 11) {
            return mitama::success(version_prefix(enable_gnu) + "11");
        } else if (cpp_version == 14) {
            if (cv < "3.2") {
                return mitama::failure("Invalid using C++14 less than clang-3.2");
            } else if (cv >= "3.2" && cv < "3.5") {
                return mitama::success(version_prefix(enable_gnu) + "1y");
            } else {
                return mitama::success(version_prefix(enable_gnu) + "14");
            }
        } else if (cpp_version == 17) {
            if (cv < "3.5.0") {
                return mitama::failure("Invalid using C++17 less than clang-3.5.0");
            } else if (cv >= "3.5.0" && cv < "5.0.0") {
                return mitama::success(version_prefix(enable_gnu) + "1z");
            } else {
                return mitama::success(version_prefix(enable_gnu) + "17");
            }
        } else if (cpp_version == 20) {
            if (cv < "6.0.0") {
                return mitama::failure("Invalid using C++20 less than clang-5.0.0");
            } else {
                return mitama::success(version_prefix(enable_gnu) + "2a");
            }
        } else {
            return mitama::failure(
                fmt::format("Unknown C++ version: {}", cpp_version)
            );
        }
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    icc_convert(const std::uint_fast8_t& cpp_version) noexcept {
        if (cpp_version == 98) {
            return mitama::success(""); // unneeded version prefix
        } else if (cpp_version == 3 || cpp_version == 11) {
#ifndef _WIN32
            return mitama::success(version_prefix(false) + "11");
#else
            return mitama::success("/Qstd:c++11");
#endif
        } else if (cpp_version == 14) {
#ifndef _WIN32
            return mitama::success(version_prefix(false) + "14");
#else
            return mitama::success("/Qstd:c++14");
#endif
        } else if (cpp_version == 17) {
#ifndef _WIN32
            return mitama::success(version_prefix(false) + "17");
#else
            return mitama::success("/Qstd:c++17");
#endif
        } else if (cpp_version == 20) {
            return mitama::failure(
                "Currently, Intel C++ Compiler does not support C++20.\n"
                "If this error is displayed in spite of C++20 is supported,\n"
                " please report the issue to https://github.com/poacpm/poac/issues."
            );
        } else {
            return mitama::failure(
                fmt::format("Unknown C++ version: {}", cpp_version)
            );
        }
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    msvc_convert(const std::uint_fast8_t& cpp_version) noexcept {
        if (cpp_version == 98 || cpp_version == 3 || cpp_version == 11) {
            return mitama::success(""); // unneeded version prefix
        } else if (cpp_version == 14) {
            return mitama::success("/std:c++14");
        } else if (cpp_version == 17) {
            return mitama::success("/std:c++17");
        } else if (cpp_version == 20) {
            return mitama::failure(
                "Currently, MSVC does not support C++20.\n"
                "If this error is displayed in spite of C++20 is supported,\n"
                " please report the issue to https://github.com/poacpm/poac/issues."
            );
        } else {
            return mitama::failure(
                fmt::format("Unknown C++ version: {}", cpp_version)
            );
        }
        // TODO: latestを活用
    }

    [[nodiscard]] mitama::result<util::cfg::compiler, std::string>
    command_to_name(std::string_view cmd) {
        if (cmd == "icpc") {
            return mitama::success(util::cfg::compiler::icc);
        }
#ifndef _WIN32
        else if (cmd == "g++" || cmd == "clang++") {
#  ifdef __APPLE__
            const std::string compiler(cmd);
            if (const auto res = util::shell::cmd(compiler + " --version").stderr_to_stdout().exec()) {
                const std::regex SEARCH("^" + ANY + "(Apple LLVM)" + ANY + "$");
                std::smatch match;
                if (std::regex_match(*res, match, SEARCH)) {
                    return mitama::success(util::cfg::compiler::apple_clang);
                }
            }
#  endif
            if (cmd == "g++") {
                return mitama::success(util::cfg::compiler::gcc);
            } else if (cmd == "clang++") {
                return mitama::success(util::cfg::compiler::clang);
            }
        }
#else
        else if (cmd == "cl.exe") {
            return mitama::success(util::cfg::compiler::msvc);
        }
#endif
        return mitama::failure(
            fmt::format("Unknown compiler command: {}", cmd)
        );
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    convert(const std::uint_fast8_t& cpp_version, const std::string& command, const bool& enable_gnu) {
        // Match a poac binary architecture and an architecture available to each compiler.
        switch (MITAMA_TRY(command_to_name(command))) {
            // Support OS: macos, linux, mingw, cygwin, _WIN32
            case util::cfg::compiler::icc:
                return icc_convert(cpp_version);
            // Support OS: macos, linux, mingw, cygwin (exclude _WIN32)
            case util::cfg::compiler::gcc:
                return gcc_convert(
                    cpp_version,
                    MITAMA_TRY(get_compiler_version("g++")),
                    enable_gnu
                );
            // Support OS: macos, linux, mingw, cygwin (exclude _WIN32)
            case util::cfg::compiler::clang:
                return clang_convert(
                    cpp_version,
                    MITAMA_TRY(get_compiler_version("clang++")),
                    enable_gnu
                );
            // Support OS: Only _WIN32
            case util::cfg::compiler::msvc:
                return msvc_convert(cpp_version);
            // Support OS: Only macos
            case util::cfg::compiler::apple_clang:
                return apple_llvm_convert(cpp_version, enable_gnu);
            default:
                return mitama::failure(
                    "To access out of range of the "
                    "enumeration values is undefined behavior."
                );
        }
    }

    [[nodiscard]] mitama::result<std::string, std::string>
    detect_command() noexcept {
        if (const auto cxx = util::misc::dupenv("CXX")) {
            return mitama::success(*cxx);
        } else if (util::shell::has_command("icpc")) {
            return mitama::success("icpc");
        }
#ifndef _WIN32
        else if (util::shell::has_command("g++")) {
            return mitama::success("g++");
        } else if (util::shell::has_command("clang++")) {
            return mitama::success("clang++");
        }
#else
        else if (util::_shell::has_command("cl.exe")) {
            return mitama::success("cl.exe");
        }
#endif
        else {
            return mitama::failure(
                "Environment variable \"CXX\" was not found.\n"
                "Select the compiler and export it."
            );
        }
    }
} // end namespace
#endif // POAC_CORE_BUILDER_STANDARD_HPP
