#ifndef POAC_CORE_BUILDER_COMPILER_CXX_HPP
#define POAC_CORE_BUILDER_COMPILER_CXX_HPP

// mod
#include <poac/core/builder/compiler/cxx/apple_clang.hpp>
#include <poac/core/builder/compiler/cxx/clang.hpp>
#include <poac/core/builder/compiler/cxx/gcc.hpp>

// external
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>

// internal
#include <poac/core/builder/compiler/error.hpp>
#include <poac/util/cfg.hpp>
#include <poac/util/misc.hpp>
#include <poac/util/shell.hpp>

namespace poac::core::builder::compiler::cxx {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using CompilerCommandNotFound =
            error<
                "either general compilers or environment variable `POAC_CXX` was not found.\n"
                "Please export it like `export POAC_CXX=g++-11`."
            >;

        using UnknownCompilerCommand =
            error<"unknown compiler command found: {}", std::string>;

        using UnsupportedCompiler =
            error<"unsupported compiler found: {}", std::string>;
    };

    inline const std::string ANY = R"([\s\S]*)";

    [[nodiscard]] anyhow::result<util::cfg::compiler>
    get_compiler_ident(const std::string& compiler_command) {
        if (compiler_command == "g++" || compiler_command == "clang++") {
#  ifdef __APPLE__
            const std::string compiler(compiler_command);
            if (const auto res = util::shell::cmd(compiler + " --version").stderr_to_stdout().exec()) {
                if (res.value().find("Apple") != std::string::npos) {
                    return mitama::success(util::cfg::compiler::apple_clang);
                }
            }
#  endif
            if (compiler_command == "g++") {
                return mitama::success(util::cfg::compiler::gcc);
            } else if (compiler_command == "clang++") {
                return mitama::success(util::cfg::compiler::clang);
            }
        }
        return anyhow::failure<Error::UnknownCompilerCommand>(compiler_command);
    }

    [[nodiscard]] anyhow::result<std::string>
    get_std_flag(
        const util::cfg::compiler compiler,
        const std::string& compiler_command,
        const std::int64_t& cpp,
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
                return anyhow::failure<Error::UnsupportedCompiler>(error::to_string(compiler));
        }
    }

    [[nodiscard]] anyhow::result<std::string>
    get_compiler_command() {
        if (const auto cxx = util::misc::dupenv("POAC_CXX")) {
            return mitama::success(cxx.value());
        } else if (util::shell::has_command("g++")) {
            return mitama::success("g++");
        } else if (util::shell::has_command("clang++")) {
            return mitama::success("clang++");
        } else {
            return anyhow::failure<Error::CompilerCommandNotFound>();
        }
    }

    [[nodiscard]] anyhow::result<std::string>
    get_command(const std::int64_t& cpp, const bool use_gnu_extension) {
        const std::string compiler_command = MITAMA_TRY(get_compiler_command());
        const util::cfg::compiler compiler = MITAMA_TRY(get_compiler_ident(compiler_command));
        const std::string std_flag = MITAMA_TRY(get_std_flag(compiler, compiler_command, cpp, use_gnu_extension));
        return mitama::success(fmt::format("{} {}", compiler_command, std_flag));
    }
} // end namespace

#endif // !POAC_CORE_BUILDER_COMPILER_CXX_HPP
