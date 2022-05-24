#ifndef POAC_CORE_BUILDER_COMPILER_ERROR_HPP_
#define POAC_CORE_BUILDER_COMPILER_ERROR_HPP_

// internal
#include "poac/poac.hh"

#include <poac/core/builder/compiler/lang.hh>
#include <poac/util/cfg.hh> // compiler

namespace poac::core::builder::compiler::error {

String
to_string(util::cfg::compiler comp) {
  switch (comp) {
    case util::cfg::compiler::gcc:
      return "GCC";
    case util::cfg::compiler::clang:
      return "Clang";
    case util::cfg::compiler::apple_clang:
      return "Apple Clang";
    case util::cfg::compiler::msvc:
      return "MSVC";
    case util::cfg::compiler::icc:
      return "Intel C++ Compiler";
    default:
      unreachable();
  }
}

std::ostream&
operator<<(std::ostream& os, util::cfg::compiler comp) {
  return (os << to_string(comp));
}

using UnsupportedLangVersion = Error<
    "`{}` ({}) does not support {} edition: `{}`", String, String, String, i64>;
using FailedToGetCompilerVersion =
    Error<"failed to get version of compiler `{}`", String>;

} // namespace poac::core::builder::compiler::error

#endif // POAC_CORE_BUILDER_COMPILER_ERROR_HPP_
