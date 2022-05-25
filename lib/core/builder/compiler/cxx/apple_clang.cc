// internal
#include "poac/core/builder/compiler/cxx/apple_clang.hpp"

#include "poac/core/builder/compiler/lang/error.hpp"
#include "poac/util/shell.hpp"

namespace poac::core::builder::compiler::cxx::apple_clang {

[[nodiscard]] Result<semver::Version>
get_compiler_version(const String& compiler_command) {
  const auto res =
      util::shell::Cmd(compiler_command + " --version").dump_stderr().exec();
  if (res.has_value()) {
    // `Apple clang version 13.0.0 (...)`
    String version;
    for (usize i = 20; i < res->size(); ++i) {
      if (res->operator[](i) == ' ') { // read until space
        break;
      }
      version += res->operator[](i);
    }
    return Ok(semver::parse(version));
  }
  return Err<error::FailedToGetCompilerVersion>(compiler);
}

// thanks to:
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/AppleClang-CXX.cmake
[[nodiscard]] Result<String>
get_std_flag(
    const String& compiler_command, const i64 edition,
    const bool use_gnu_extension
) {
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
      compiler, version, lang::Lang::cxx, edition
  );
}

} // namespace poac::core::builder::compiler::cxx::apple_clang
