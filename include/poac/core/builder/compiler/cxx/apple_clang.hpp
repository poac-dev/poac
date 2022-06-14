#ifndef POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP_
#define POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP_

// internal
#include "poac/poac.hpp"
#include "poac/util/cfg.hpp"
#include "poac/util/semver/semver.hpp"

namespace poac::core::builder::compiler::cxx::apple_clang {

inline constexpr util::cfg::compiler compiler =
    util::cfg::compiler::apple_clang;

[[nodiscard]] Result<semver::Version>
get_compiler_version_impl(const String& cmd_output);

[[nodiscard]] Result<semver::Version>
get_compiler_version(const String& compiler_command);

// thanks to:
// https://gitlab.kitware.com/cmake/cmake/-/blob/master/Modules/Compiler/AppleClang-CXX.cmake
[[nodiscard]] Result<String>
get_std_flag(
    const String& compiler_command, const i64 edition,
    const bool use_gnu_extension
);

} // namespace poac::core::builder::compiler::cxx::apple_clang

#endif // POAC_CORE_BUILDER_COMPILER_CXX_APPLE_CLANG_HPP_
