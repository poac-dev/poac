module;

// std
#include <string>

// internal
#include "../../../../util/result-macros.hpp"

export module poac.core.builder.compiler.cxx.apple_clang;

import poac.core.builder.compiler.error;
import poac.core.builder.compiler.lang;
import poac.util.cfg;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;
import poac.util.shell;
import semver;

namespace poac::core::builder::compiler::cxx::apple_clang {

inline constexpr util::cfg::Compiler COMPILER =
    util::cfg::Compiler::apple_clang;

[[nodiscard]] auto get_compiler_version_impl(const String& cmd_output)
    -> Result<semver::Version> {
  // `Apple clang version 13.0.0 (...)`
  String version;
  for (usize i = 20; i < cmd_output.size(); ++i) {
    if (cmd_output[i] == ' ') { // read until space
      break;
    }
    version += cmd_output[i];
  }
  return Ok(semver::parse(version));
}

[[nodiscard]] auto get_compiler_version(const String& compiler_command)
    -> Result<semver::Version> {
  const auto res =
      util::shell::Cmd(compiler_command + " --version").dump_stderr().exec();
  if (res.is_ok()) {
    return get_compiler_version_impl(res.output());
  }
  return Err<error::FailedToGetCompilerVersion>(COMPILER);
}

// thanks to:
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/AppleClang-CXX.cmake
export [[nodiscard]] auto get_std_flag(
    const String& compiler_command, i64 edition, bool use_gnu_extension
) -> Result<String> {
  const semver::Version version = Try(get_compiler_version(compiler_command));
  const String specifier = use_gnu_extension ? "gnu" : "c";
  switch (edition) {
    case 1998:
      if (version >= "4.0.0") {
        return Ok(format("-std={}++98", specifier));
      }
      break;
    case 2011:
      if (version >= "4.0.0") {
        return Ok(format("-std={}++11", specifier));
      }
      break;
    case 2014:
      if (version >= "6.1.0") {
        return Ok(format("-std={}++14", specifier));
      } else if (version >= "5.1.0") {
        // AppleClang 5.0 knows this flag, but does not set a __cplusplus macro
        // greater than 201103L
        return Ok(format("-std={}++1y", specifier));
      }
      break;
    case 2017:
      if (version >= "10.0.0") {
        return Ok(format("-std={}++17", specifier));
      } else if (version >= "6.1.0") {
        return Ok(format("-std={}++1z", specifier));
      }
      break;
    case 2020:
      if (version >= "13.0.0") {
        return Ok(format("-std={}++20", specifier));
      } else if (version >= "10.0.0") {
        return Ok(format("-std={}++2a", specifier));
      }
      break;
  }
  return Err<error::UnsupportedLangVersion>(
      COMPILER, version, lang::Lang::cxx, edition
  );
}

} // namespace poac::core::builder::compiler::cxx::apple_clang
