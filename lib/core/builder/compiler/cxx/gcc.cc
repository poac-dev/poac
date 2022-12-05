// std
#include <cctype> // std::isdigit

// internal
#include "poac/core/builder/compiler/cxx/gcc.hpp"
#include "poac/core/builder/compiler/error.hpp"
#include "poac/util/shell.hpp"

namespace poac::core::builder::compiler::cxx::gcc {

[[nodiscard]] auto get_compiler_version_impl(const String& cmd_output)
    -> Result<semver::Version> {
  // `g++ (GCC) 11.2.0\n`
  usize itr = cmd_output.find('(');
  if (itr == None) {
    return Err<error::FailedToGetCompilerVersion>(compiler);
  }
  itr = cmd_output.find(')', itr + 1);

  String version;
  for (itr += 2; itr < cmd_output.size(); ++itr) {
    if (std::isdigit(cmd_output[itr]) || cmd_output[itr] == '.') {
      version += cmd_output[itr];
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
  return Err<error::FailedToGetCompilerVersion>(compiler);
}

// thanks to:
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/GNU-CXX.cmake
[[nodiscard]] auto get_std_flag(
    const String& compiler_command, const i64 edition,
    const bool use_gnu_extension
) -> Result<String> {
  const semver::Version version = Try(get_compiler_version(compiler_command));
  const String specifier = use_gnu_extension ? "gnu" : "c";
  switch (edition) {
    case 1998:
      if (version >= "3.4.0") {
        return Ok(format("-std={}++98", specifier));
      }
      break;
    case 2011:
      if (version >= "4.7.0") {
        return Ok(format("-std={}++11", specifier));
      } else if (version >= "4.4.0") {
        return Ok(format("-std={}++0x", specifier));
      }
      break;
    case 2014:
      if (version >= "4.9.0") {
        return Ok(format("-std={}++14", specifier));
      } else if (version >= "4.8.0") {
        return Ok(format("-std={}++1y", specifier));
      }
      break;
    case 2017:
      if (version >= "8.0.0") {
        return Ok(format("-std={}++17", specifier));
      } else if (version >= "5.1.0") {
        return Ok(format("-std={}++1z", specifier));
      }
      break;
    case 2020:
      if (version >= "11.1.0") {
        return Ok(format("-std={}++20", specifier));
      } else if (version >= "8.0.0") {
        return Ok(format("-std={}++2a", specifier));
      }
      break;
  }
  return Err<error::UnsupportedLangVersion>(
      compiler, version, lang::Lang::cxx, edition
  );
}

} // namespace poac::core::builder::compiler::cxx::gcc
