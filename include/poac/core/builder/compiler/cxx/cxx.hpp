#ifndef POAC_CORE_BUILDER_COMPILER_CXX_CXX_HPP_
#define POAC_CORE_BUILDER_COMPILER_CXX_CXX_HPP_

// internal
#include "poac/poac.hpp"
#include "poac/util/cfg.hpp"

namespace poac::core::builder::compiler::cxx {

using CompilerCommandNotFound = Error<
    "either general compilers or environment variable `POAC_CXX` was not found.\n"
    "Please export it like `export POAC_CXX=g++-11`.">;
using UnknownCompilerCommand =
    Error<"unknown compiler command found: {}", String>;
using UnsupportedCompiler = Error<"unsupported compiler found: {}", String>;

[[nodiscard]] Fn
get_compiler_ident(const String& compiler_command, bool is_macos)
    ->Result<util::cfg::Compiler>;

[[nodiscard]] Fn get_std_flag(
    util::cfg::Compiler compiler, const String& compiler_command, i64 edition,
    bool use_gnu_extension
)
    ->Result<String>;

[[nodiscard]] Fn get_compiler_command()->Result<String>;

[[nodiscard]] Fn get_command(i64 edition, bool use_gnu_extension)
    ->Result<String>;

} // namespace poac::core::builder::compiler::cxx

#endif // POAC_CORE_BUILDER_COMPILER_CXX_CXX_HPP_
