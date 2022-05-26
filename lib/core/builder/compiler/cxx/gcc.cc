// internal
#include "poac/core/builder/compiler/cxx/gcc.hpp"

#include "poac/core/builder/compiler/lang/error.hpp"
#include "poac/util/shell.hpp"

namespace poac::core::builder::compiler::cxx::gcc {

[[nodiscard]] Result<semver::Version>
get_compiler_version(const String& compiler_command) {
  const auto res = util::shell::Cmd(compiler_command + " --version").exec();
  if (res.is_ok()) {
    // `g++ (GCC) 11.2.0\n`
    const String output = res.output();
    String version;
    for (usize i = 10; i < output.size(); ++i) {
      if (output[i] == '\n') { // read until '\n'
        break;
      }
      version += output[i];
    }
    return Ok(semver::parse(version));
  }
  return Err<error::FailedToGetCompilerVersion>(compiler);
}

// thanks to:
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/GNU-CXX.cmake
[[nodiscard]] Result<String>
get_std_flag(
    const String& compiler_command, const i64 edition,
    const bool use_gnu_extension
) {
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
