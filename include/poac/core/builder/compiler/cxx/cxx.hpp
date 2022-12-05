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

[[nodiscard]] Result<util::cfg::compiler>
get_compiler_ident(const String& compiler_command, const bool is_macos);

[[nodiscard]] Result<String> get_std_flag(
    const util::cfg::compiler compiler, const String& compiler_command,
    const i64 edition, const bool use_gnu_extension
);

[[nodiscard]] Result<String> get_compiler_command();

[[nodiscard]] Result<String>
get_command(const i64 edition, const bool use_gnu_extension);

} // namespace poac::core::builder::compiler::cxx

#endif // POAC_CORE_BUILDER_COMPILER_CXX_CXX_HPP_
