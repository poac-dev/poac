// internal
#include "poac/core/builder/compiler/cxx/cxx.hpp"

#include "poac/core/builder/compiler/cxx/apple_clang.hpp"
#include "poac/core/builder/compiler/cxx/clang.hpp"
#include "poac/core/builder/compiler/cxx/gcc.hpp"
#include "poac/core/builder/compiler/lang/error.hpp"
#include "poac/util/misc.hpp"
#include "poac/util/shell.hpp"

namespace poac::core::builder::compiler::cxx {

[[nodiscard]] Result<util::cfg::compiler>
get_compiler_ident(const String& compiler_command) {
#ifdef __APPLE__
  if (const auto res = util::shell::Cmd(compiler_command + " --version")
                           .stderr_to_stdout()
                           .exec()) {
    if (res.output().find("Apple") != SNone) {
      return Ok(util::cfg::compiler::apple_clang);
    }
  }
#endif

  if (compiler_command.find("g++") != SNone) {
    return Ok(util::cfg::compiler::gcc);
  }
  if (compiler_command.find("clang++") != SNone) {
    return Ok(util::cfg::compiler::clang);
  }
  return Err<UnknownCompilerCommand>(compiler_command);
}

[[nodiscard]] Result<String>
get_std_flag(
    const util::cfg::compiler compiler, const String& compiler_command,
    const i64 edition, const bool use_gnu_extension
) {
  switch (compiler) {
    case util::cfg::compiler::gcc:
      return gcc::get_std_flag(compiler_command, edition, use_gnu_extension);
    case util::cfg::compiler::clang:
      return clang::get_std_flag(compiler_command, edition, use_gnu_extension);
    case util::cfg::compiler::apple_clang:
      return apple_clang::get_std_flag(
          compiler_command, edition, use_gnu_extension
      );
    default:
      return Err<UnsupportedCompiler>(error::to_string(compiler));
  }
}

[[nodiscard]] Result<String>
get_compiler_command() {
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

[[nodiscard]] Result<String>
get_command(const i64 edition, const bool use_gnu_extension) {
  const String compiler_command = Try(get_compiler_command());
  const util::cfg::compiler compiler =
      Try(get_compiler_ident(compiler_command));
  const String std_flag =
      Try(get_std_flag(compiler, compiler_command, edition, use_gnu_extension));
  return Ok(format("{} {}", compiler_command, std_flag));
}

} // namespace poac::core::builder::compiler::cxx
