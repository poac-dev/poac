#ifndef POAC_CORE_STROITE_FIELD_STANDARD_HPP
#define POAC_CORE_STROITE_FIELD_STANDARD_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <regex>

#include "../../deper/semver.hpp"
#include "../../except.hpp"
#include "../../../util/command.hpp"


namespace poac::core::stroite::field::standard {
    inline std::string version_prefix(const bool& enable_gnu) noexcept {
        return enable_gnu ? "-std=gnu++" : "-std=c++";
    }

    std::string get_compiler_version(const std::string& compiler) {
        if (util::_command::has_command(compiler)) {
            if (const auto res = util::command(compiler + " --version").stderr_to_stdout().exec()) {
                const std::string ANY = R"([\s\S]*)";
                const std::regex SEARCH_VERSION("^" + ANY + "(" + deper::semver::MAIN_VERSION + ")" + ANY + "$");

                std::smatch match;
                if (std::regex_match(*res, match, SEARCH_VERSION)) {
                    return match[1];
                }
            }
        }
        throw except::error("Could not get " + compiler + " version");
    }

    std::string apple_llvm_convert(const std::uint8_t& cpp_version, const bool& enable_gnu) {
        if (cpp_version == 98) {
            return version_prefix(enable_gnu) + "98";
        }
        else if (cpp_version == 3 || cpp_version == 11) {
            return version_prefix(enable_gnu) + "11";
        }
        else if (cpp_version == 14) {
            return version_prefix(enable_gnu) + "14";
        }
        else if (cpp_version == 17) {
            return version_prefix(enable_gnu) + "17";
        }
        else if (cpp_version == 20) {
            throw except::error("Currently, Apple LLVM does not support C++20.\n"
                                "If this error is displayed in spite of C++20 is supported,\n"
                                " please report the issue to https://github.com/poacpm/poac/issues.");
        }
        else {
            throw except::error("Unknown C++ version: " + std::to_string(cpp_version));
        }
    }

    std::string gcc_convert(const std::uint8_t& cpp_version, const std::string& compiler_version, const bool& enable_gnu) {
        const deper::semver::Version cv(compiler_version);

        if (cpp_version == 98) {
            return ""; // unneeded version prefix
        }
        else if (cpp_version == 3 || cpp_version == 11) {
            if (cv < "4.3") {
                throw except::error("Invalid using C++11 less than gcc-4.3.0");
            }
            else if (cv >= "4.3" && cv < "4.7") {
                return version_prefix(enable_gnu) + "0x";
            }
            else {
                return version_prefix(enable_gnu) + "11";
            }
        }
        else if (cpp_version == 14) {
            if (cv < "4.8") {
                throw except::error("Invalid using C++14 less than gcc-4.8.0");
            }
            else if (cv >= "4.8" && cv < "4.9") {
                return version_prefix(enable_gnu) + "1y";
            }
            else {
                return version_prefix(enable_gnu) + "14";
            }
        }
        else if (cpp_version == 17) {
            if (cv < "5") {
                throw except::error("Invalid using C++17 less than gcc-5.0.0");
            }
            else {
                return version_prefix(enable_gnu) + "17";
            }
        }
        else if (cpp_version == 20) {
            if (cv < "8") {
                throw except::error("Invalid using C++20 less than gcc-8.0.0");
            }
            else {
                return version_prefix(enable_gnu) + "2a";
            }
        }
        else {
            throw except::error("Unknown C++ version: " + std::to_string(cpp_version));
        }
    }

    std::string clang_convert(const std::uint8_t& cpp_version, const std::string& compiler_version, const bool& enable_gnu) {
        const deper::semver::Version cv(compiler_version);

        if (cpp_version == 98) {
            return ""; // unneeded version prefix
        }
        else if (cpp_version == 3 || cpp_version == 11) {
            return version_prefix(enable_gnu) + "11";
        }
        else if (cpp_version == 14) {
            if (cv < "3.2") {
                throw except::error("Invalid using C++14 less than clang-3.2");
            }
            else if (cv >= "3.2" && cv < "3.5") {
                return version_prefix(enable_gnu) + "1y";
            }
            else {
                return version_prefix(enable_gnu) + "14";
            }
        }
        else if (cpp_version == 17) {
            if (cv < "3.5.0") {
                throw except::error("Invalid using C++17 less than clang-3.5.0");
            }
            else if (cv >= "3.5.0" && cv < "5.0.0") {
                return version_prefix(enable_gnu) + "1z";
            }
            else {
                return version_prefix(enable_gnu) + "17";
            }
        }
        else if (cpp_version == 20) {
            if (cv < "6.0.0") {
                throw except::error("Invalid using C++20 less than clang-5.0.0");
            }
            else {
                return version_prefix(enable_gnu) + "2a";
            }
        }
        else {
            throw except::error("Unknown C++ version: " + std::to_string(cpp_version));
        }
    }

    std::string icc_convert(const std::uint8_t& cpp_version) {
        if (cpp_version == 98) {
            return ""; // unneeded version prefix
        }
        else if (cpp_version == 3 || cpp_version == 11) {
            return version_prefix(false) + "11";
        }
        else if (cpp_version == 14) {
            return version_prefix(false) + "14";
        }
        else if (cpp_version == 17) {
            return version_prefix(false) + "17";
        }
        else if (cpp_version == 20) {
            throw except::error("Currently, Intel C++ Compiler does not support C++20.\n"
                                "If this error is displayed in spite of C++20 is supported,\n"
                                " please report the issue to https://github.com/poacpm/poac/issues.");
        }
        else {
            throw except::error("Unknown C++ version: " + std::to_string(cpp_version));
        }
    }

    std::string convert(const std::uint8_t& cpp_version, std::string_view compiler, const bool& enable_gnu) {
        // Apple LLVMは，コンパイラバージョンによる差異がないものとする．(半ば強制的にupdateされるため)
        if (compiler == "apple-llvm") {
            // TODO: 判断では，g++等の--versionに，Apple LLVMという文言が含まれているかどうか．
            // TODO: この，apple-llvmとかは，コマンド名に直接使用できない．-> つまり，detect.hppの内容が使用できない．
            return apple_llvm_convert(cpp_version, enable_gnu);
        }
        else if (compiler == "gcc") {
            return gcc_convert(cpp_version, get_compiler_version("g++"), enable_gnu);
        }
        else if (compiler == "clang") {
            return clang_convert(cpp_version, get_compiler_version("clang++"), enable_gnu);
        }
        else if (compiler == "icc") {
            return icc_convert(cpp_version);
        } // TODO: WIN32環境下で動作させられるのは，msvcとiccのみ．
        else if (compiler == "msvc") { //TODO: WIN32環境（MinGWとかCygwinじゃなく）は，どちらにせよ，そのバイナリしか動作しないため，preprocess時に決めてしまってよい．

        }
        return "";
    }
} // end namespace
#endif // POAC_CORE_STROITE_FIELD_STANDARD_HPP
