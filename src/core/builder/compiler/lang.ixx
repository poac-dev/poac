module;

// external
#include <fmt/format.h>

export module poac.core.builder.compiler.lang;

import poac.util.cfg; // compiler
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;

export namespace poac::core::builder::compiler::lang {

enum class Lang {
  c,
  cxx,
};

auto to_string(Lang lang) -> String {
  switch (lang) {
    case Lang::c:
      return "C";
    case Lang::cxx:
      return "C++";
    default:
      __builtin_unreachable();
  }
}

} // namespace poac::core::builder::compiler::lang

namespace fmt {

template <>
struct formatter<poac::core::builder::compiler::lang::Lang> {
  static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline auto
  format(poac::core::builder::compiler::lang::Lang l, FormatContext& ctx) {
    return format_to(
        ctx.out(), "{}", poac::core::builder::compiler::lang::to_string(l)
    );
  }
};

} // namespace fmt
