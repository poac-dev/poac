module;

// internal
#include "../../../../util/result-macros.hpp"

export module poac.core.builder.compiler.cxx.cxx;

export import poac.core.builder.compiler.cxx.apple_clang;
export import poac.core.builder.compiler.cxx.clang;
export import poac.core.builder.compiler.cxx.gcc;
export import poac.core.builder.compiler.error;

import poac.util.cfg;
import poac.util.format;
import poac.util.log;
import poac.util.misc;
import poac.util.result;
import poac.util.rustify;
import poac.util.shell;

namespace poac::core::builder::compiler::cxx {

using CompilerCommandNotFound = Error<
    "either general compilers or environment variable `POAC_CXX` was not found.\n"
    "Please export it like `export POAC_CXX=g++-11`.">;
using UnknownCompilerCommand =
    Error<"unknown compiler command found: {}", String>;
using UnsupportedCompiler = Error<"unsupported compiler found: {}", String>;

[[nodiscard]] auto
get_compiler_ident(const String& compiler_command, bool is_macos)
    -> Result<util::cfg::Compiler> {
  if (is_macos) {
    if (const auto res = util::shell::Cmd(compiler_command + " --version")
                      .stderr_to_stdout()
                      .exec()) {
      if (res.output().find("Apple") != None) {
        return Ok(util::cfg::Compiler::apple_clang);
      }
    }
  }

  // `clang++` should be before `g++` because `g++` is a part of `clang++`
  if (compiler_command.find("clang++") != None) {
    return Ok(util::cfg::Compiler::clang);
  }
  if (compiler_command.find("g++") != None) {
    return Ok(util::cfg::Compiler::gcc);
  }
  return Err<UnknownCompilerCommand>(compiler_command);
}

[[nodiscard]] auto get_std_flag(
    util::cfg::Compiler compiler, const String& compiler_command, i64 edition,
    bool use_gnu_extension
) -> Result<String> {
  switch (compiler) {
    case util::cfg::Compiler::gcc:
      return gcc::get_std_flag(compiler_command, edition, use_gnu_extension);
    case util::cfg::Compiler::clang:
      return clang::get_std_flag(compiler_command, edition, use_gnu_extension);
    case util::cfg::Compiler::apple_clang:
      return apple_clang::get_std_flag(
          compiler_command, edition, use_gnu_extension
      );
    default:
      return Err<UnsupportedCompiler>(error::to_string(compiler));
  }
}

[[nodiscard]] auto get_compiler_command() -> Result<String> {
  if (const auto cxx = util::misc::dupenv("CXX")) {
    return Ok(cxx.value());
  } else if (util::shell::has_command("g++")) {
    return Ok("g++");
  } else if (util::shell::has_command("clang++")) {
    return Ok("clang++");
  } else {
    return Err<CompilerCommandNotFound>();
  }
}

export [[nodiscard]] auto get_command(i64 edition, bool use_gnu_extension)
    -> Result<String> {
  const String compiler_command = Try(get_compiler_command());
  const util::cfg::Compiler compiler =
#ifdef __APPLE__
      Try(get_compiler_ident(compiler_command, true));
#else
      Try(get_compiler_ident(compiler_command, false));
#endif

  const String std_flag =
      Try(get_std_flag(compiler, compiler_command, edition, use_gnu_extension));
  return Ok(format("{} {}", compiler_command, std_flag));
}

} // namespace poac::core::builder::compiler::cxx
