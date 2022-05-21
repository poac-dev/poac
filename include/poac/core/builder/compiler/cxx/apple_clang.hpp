#ifndef POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP
#define POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP

// internal
#include <poac/core/builder/compiler/error.hpp>
#include <poac/poac.hpp>
#include <poac/util/cfg.hpp>
#include <poac/util/semver/semver.hpp>
#include <poac/util/shell.hpp>

namespace poac::core::builder::compiler::cxx::apple_clang {

inline constexpr util::cfg::compiler compiler =
    util::cfg::compiler::apple_clang;

[[nodiscard]] Result<semver::Version>
get_compiler_version(const String& compiler_command) {
  const auto res =
      util::shell::Cmd(compiler_command + " --version").dump_stderr().exec();
  if (res.has_value()) {
    // `Apple clang version 13.0.0 (...)`
    String version;
    for (usize i = 20; i < res.value().size(); ++i) {
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
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/AppleClang-CXX.cmake
[[nodiscard]] Result<String>
get_std_flag(
    const String& compiler_command, const i64& cpp, const bool use_gnu_extension
) {
  const semver::Version version = Try(get_compiler_version(compiler_command));
  const String specifier = use_gnu_extension ? "gnu" : "c";
  switch (cpp) {
    case 98:
      if (version >= "4.0.0") {
        return Ok(format("-std={}++98", specifier));
      }
      break;
    case 11:
      if (version >= "4.0.0") {
        return Ok(format("-std={}++11", specifier));
      }
      break;
    case 14:
      if (version >= "6.1.0") {
        return Ok(format("-std={}++14", specifier));
      } else if (version >= "5.1.0") {
        // AppleClang 5.0 knows this flag, but does not set a __cplusplus macro
        // greater than 201103L
        return Ok(format("-std={}++1y", specifier));
      }
      break;
    case 17:
      if (version >= "10.0.0") {
        return Ok(format("-std={}++17", specifier));
      } else if (version >= "6.1.0") {
        return Ok(format("-std={}++1z", specifier));
      }
      break;
    case 20:
      if (version >= "13.0.0") {
        return Ok(format("-std={}++20", specifier));
      } else if (version >= "10.0.0") {
        return Ok(format("-std={}++2a", specifier));
      }
      break;
  }
  return Err<error::UnsupportedLangVersion>(
      error::to_string(compiler), version.get_full(),
      to_string(lang::Lang::cxx), cpp
  );
}

} // namespace poac::core::builder::compiler::cxx::apple_clang

#endif // !POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP
