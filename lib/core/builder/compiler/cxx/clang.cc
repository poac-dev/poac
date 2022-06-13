// std
#include <cctype> // std::isdigit

// internal
#include "poac/core/builder/compiler/cxx/clang.hpp"
#include "poac/core/builder/compiler/lang/error.hpp"
#include "poac/util/shell.hpp"

namespace poac::core::builder::compiler::cxx::clang {

[[nodiscard]] Result<semver::Version>
get_compiler_version(const String& compiler_command) {
  const auto res = util::shell::Cmd(compiler_command + " --version").exec();
  if (res.is_ok()) {
    // `clang version 12.0.0 (...)`
    const String output = res.output();
    String search = "version ";
    usize i = output.find(search);
    if (i == SNone) {
      return Err<error::FailedToGetCompilerVersion>(compiler);
    }

    String version;
    for (i += search.size(); i < output.size(); ++i) {
      if (std::isdigit(output[i]) || output[i] == '.') {
        version += output[i];
      } else {
        break;
      }
    }
    return Ok(semver::parse(version));
  }
  return Err<error::FailedToGetCompilerVersion>(compiler);
}

// thanks to:
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/Clang.cmake
[[nodiscard]] Result<String>
get_std_flag(
    const String& compiler_command, const i64 edition,
    const bool use_gnu_extension
) {
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
      compiler, version, lang::Lang::cxx, edition
  );
}

} // namespace poac::core::builder::compiler::cxx::clang
