#pragma once

// internal
#include "poac/poac.hpp"
#include "poac/util/cfg.hpp"
#include "poac/util/semver/semver.hpp"

namespace poac::core::builder::compiler::cxx::gcc {

inline constexpr util::cfg::Compiler COMPILER = util::cfg::Compiler::gcc;

[[nodiscard]] Fn get_compiler_version_impl(const String& cmd_output)
    ->Result<semver::Version>;

[[nodiscard]] Fn get_compiler_version(const String& compiler_command)
    ->Result<semver::Version>;

// thanks to:
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/GNU-CXX.cmake
[[nodiscard]] Fn get_std_flag(
    const String& compiler_command, i64 edition, bool use_gnu_extension
)
    ->Result<String>;

} // namespace poac::core::builder::compiler::cxx::gcc
