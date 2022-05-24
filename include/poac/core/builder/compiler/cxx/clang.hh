#ifndef POAC_CORE_BUILDER_COMPILER_CXX_CLANG_HPP_
#define POAC_CORE_BUILDER_COMPILER_CXX_CLANG_HPP_

// internal
#include "poac/poac.hh"
#include "poac/util/semver/semver.hh"

#include <poac/core/builder/compiler/error.hh>
#include <poac/util/cfg.hh>
#include <poac/util/shell.hh>

namespace poac::core::builder::compiler::cxx::clang {

inline constexpr util::cfg::compiler compiler = util::cfg::compiler::clang;

[[nodiscard]] Result<semver::Version>
get_compiler_version(const String& compiler_command) {
  const auto res = util::shell::Cmd(compiler_command + " --version").exec();
  if (res.has_value()) {
    // `clang version 12.0.0 (...)`
    String version;
    for (usize i = 14; i < res.value().size(); ++i) {
      if (res->operator[](i) == ' ') { // read until space
        break;
      }
      version += res->operator[](i);
    }
    return Ok(semver::parse(version));
  }
  return Err<error::FailedToGetCompilerVersion>(error::to_string(compiler));
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
      error::to_string(compiler), version.get_full(),
      to_string(lang::Lang::cxx), edition
  );
}

} // namespace poac::core::builder::compiler::cxx::clang

#endif // POAC_CORE_BUILDER_COMPILER_CXX_CLANG_HPP_
