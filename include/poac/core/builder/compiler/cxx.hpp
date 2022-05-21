#ifndef POAC_CORE_BUILDER_COMPILER_CXX_HPP
#define POAC_CORE_BUILDER_COMPILER_CXX_HPP

// mod
#include <poac/core/builder/compiler/cxx/apple_clang.hpp>
#include <poac/core/builder/compiler/cxx/clang.hpp>
#include <poac/core/builder/compiler/cxx/gcc.hpp>

// internal
#include <poac/poac.hpp>
#include <poac/core/builder/compiler/error.hpp>
#include <poac/util/cfg.hpp>
#include <poac/util/misc.hpp>
#include <poac/util/shell.hpp>

namespace poac::core::builder::compiler::cxx {
    using CompilerCommandNotFound =
        Error<
            "either general compilers or environment variable `POAC_CXX` was not found.\n"
            "Please export it like `export POAC_CXX=g++-11`."
        >;
    using UnknownCompilerCommand =
        Error<"unknown compiler command found: {}", String>;
    using UnsupportedCompiler =
        Error<"unsupported compiler found: {}", String>;

    inline const String ANY = R"([\s\S]*)";

    [[nodiscard]] Result<util::cfg::compiler>
    get_compiler_ident(const String& compiler_command) {
        if (compiler_command == "g++" || compiler_command == "clang++") {
#  ifdef __APPLE__
            const String compiler(compiler_command);
            if (const auto res = util::shell::Cmd(compiler + " --version").stderr_to_stdout().exec()) {
                if (res.value().find("Apple") != SNone) {
                    return Ok(util::cfg::compiler::apple_clang);
                }
            }
#  endif
            if (compiler_command == "g++") {
                return Ok(util::cfg::compiler::gcc);
            } else if (compiler_command == "clang++") {
                return Ok(util::cfg::compiler::clang);
            }
        }
        return Err<UnknownCompilerCommand>(compiler_command);
    }

    [[nodiscard]] Result<String>
    get_std_flag(
        const util::cfg::compiler compiler,
        const String& compiler_command,
        const i64& cpp,
        const bool use_gnu_extension
    ) {
        switch (compiler) {
            case util::cfg::compiler::gcc:
                return gcc::get_std_flag(compiler_command, cpp, use_gnu_extension);
            case util::cfg::compiler::clang:
                return clang::get_std_flag(compiler_command, cpp, use_gnu_extension);
            case util::cfg::compiler::apple_clang:
                return apple_clang::get_std_flag(compiler_command, cpp, use_gnu_extension);
            default:
                return Err<UnsupportedCompiler>(error::to_string(compiler));
        }
    }

    [[nodiscard]] Result<String>
    get_compiler_command() {
        if (const auto cxx = util::misc::dupenv("POAC_CXX")) {
            return Ok(cxx.value());
        } else if (util::shell::has_command("g++")) {
            return Ok("g++");
        } else if (util::shell::has_command("clang++")) {
            return Ok("clang++");
        } else {
            return Err<CompilerCommandNotFound>();
        }
    }

    [[nodiscard]] Result<String>
    get_command(const i64& cpp, const bool use_gnu_extension) {
        const String compiler_command = Try(get_compiler_command());
        const util::cfg::compiler compiler = Try(get_compiler_ident(compiler_command));
        const String std_flag = Try(get_std_flag(compiler, compiler_command, cpp, use_gnu_extension));
        return Ok(format("{} {}", compiler_command, std_flag));
    }
} // end namespace

#endif // !POAC_CORE_BUILDER_COMPILER_CXX_HPP
