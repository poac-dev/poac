module;

// external
#include <fmt/format.h>

export module poac.core.builder.compiler.error;

import semver.token;
import poac.core.builder.compiler.lang;
import poac.util.cfg; // compiler
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;

export namespace poac::core::builder::compiler::error {

auto to_string(util::cfg::Compiler comp) -> String {
  switch (comp) {
    case util::cfg::Compiler::gcc:
      return "GCC";
    case util::cfg::Compiler::clang:
      return "Clang";
    case util::cfg::Compiler::apple_clang:
      return "Apple Clang";
    case util::cfg::Compiler::msvc:
      return "MSVC";
    case util::cfg::Compiler::icc:
      return "Intel C++ Compiler";
    default:
      __builtin_unreachable();
  }
}

} // namespace poac::core::builder::compiler::error

namespace fmt {

template <>
struct formatter<poac::util::cfg::Compiler> {
  static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline auto format(poac::util::cfg::Compiler c, FormatContext& ctx) {
    return format_to(
        ctx.out(), "{}", poac::core::builder::compiler::error::to_string(c)
    );
  }
};

} // namespace fmt

export namespace poac::core::builder::compiler::error {

using UnsupportedLangVersion = Error<
    "`{}` ({}) does not support {} edition: `{}`", util::cfg::Compiler,
    semver::Version, poac::core::builder::compiler::lang::Lang, i64>;
using FailedToGetCompilerVersion =
    Error<"failed to get version of compiler `{}`", util::cfg::Compiler>;

} // namespace poac::core::builder::compiler::error
