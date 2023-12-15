module;

// std
#include <cctype> // std::isdigit
#include <string>

// internal
#include "../../../../util/result-macros.hpp"

export module poac.core.builder.compiler.cxx.clang;

import poac.core.builder.compiler.error;
import poac.core.builder.compiler.lang;
import poac.util.shell;
import poac.util.cfg;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;
import semver;

namespace poac::core::builder::compiler::cxx::clang {

inline constexpr util::cfg::Compiler COMPILER = util::cfg::Compiler::clang;

[[nodiscard]] auto get_compiler_version_impl(const String& cmd_output)
    -> Result<semver::Version> {
  // `clang version 12.0.0 (...)`
  const String search = "version ";
  usize i = cmd_output.find(search);
  if (i == None) {
    return Err<error::FailedToGetCompilerVersion>(COMPILER);
  }

  String version;
  for (i += search.size(); i < cmd_output.size(); ++i) {
    if (std::isdigit(cmd_output[i]) || cmd_output[i] == '.') {
      version += cmd_output[i];
    } else {
      break;
    }
  }
  return Ok(semver::parse(version));
}

[[nodiscard]] auto get_compiler_version(const String& compiler_command)
    -> Result<semver::Version> {
  const auto res = util::shell::Cmd(compiler_command + " --version").exec();
  if (res.is_ok()) {
    return get_compiler_version_impl(res.output());
  }
  return Err<error::FailedToGetCompilerVersion>(COMPILER);
}

// thanks to:
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/Clang.cmake
export [[nodiscard]] auto get_std_flag(
    const String& compiler_command, i64 edition, bool use_gnu_extension
) -> Result<String> {
  const semver::Version version = Try(get_compiler_version(compiler_command));
  const String specifier = use_gnu_extension ? "gnu" : "c";
  switch (edition) {
    case 1998:
      if (version > "2.1.0") {
        return Ok(format("-std={}++98", specifier));
      }
      break;
    case 2011:
      if (version > "3.1.0") {
        return Ok(format("-std={}++11", specifier));
      } else if (version > "2.1.0") {
        return Ok(format("-std={}++0x", specifier));
      }
      break;
    case 2014:
      if (version >= "3.5.0") {
        return Ok(format("-std={}++14", specifier));
      } else if (version >= "3.4.0") {
        return Ok(format("-std={}++1y", specifier));
      }
      break;
    case 2017:
      if (version >= "5.0.0") {
        return Ok(format("-std={}++17", specifier));
      } else if (version >= "3.5.0") {
        return Ok(format("-std={}++1z", specifier));
      }
      break;
    case 2020:
      if (version >= "11.0.0") {
        return Ok(format("-std={}++20", specifier));
      } else if (version >= "5.0.0") {
        return Ok(format("-std={}++2a", specifier));
      }
      break;
  }
  return Err<error::UnsupportedLangVersion>(
      COMPILER, version, lang::Lang::cxx, edition
  );
}

} // namespace poac::core::builder::compiler::cxx::clang
