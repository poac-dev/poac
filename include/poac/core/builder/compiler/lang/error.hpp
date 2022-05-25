#ifndef POAC_CORE_BUILDER_COMPILER_LANG_ERROR_HPP_
#define POAC_CORE_BUILDER_COMPILER_LANG_ERROR_HPP_

// internal
#include "poac/core/builder/compiler/lang/lang.hpp"
#include "poac/poac.hpp"
#include "poac/util/cfg.hpp" // compiler

namespace poac::core::builder::compiler::error {

String
to_string(util::cfg::compiler comp);

inline std::ostream&
operator<<(std::ostream& os, util::cfg::compiler comp) {
  return (os << to_string(comp));
}

using UnsupportedLangVersion = Error<
    "`{}` ({}) does not support {} edition: `{}`", String, String, String, i64>;
using FailedToGetCompilerVersion =
    Error<"failed to get version of compiler `{}`", String>;

} // namespace poac::core::builder::compiler::error

#endif // POAC_CORE_BUILDER_COMPILER_LANG_ERROR_HPP_
