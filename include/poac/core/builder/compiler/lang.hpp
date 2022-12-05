#ifndef POAC_CORE_BUILDER_COMPILER_LANG_HPP_
#define POAC_CORE_BUILDER_COMPILER_LANG_HPP_

// internal
#include "poac/poac.hpp"
#include "poac/util/cfg.hpp" // compiler

namespace poac::core::builder::compiler::lang {

enum class Lang {
  c,
  cxx,
};

String to_string(Lang lang);

} // namespace poac::core::builder::compiler::lang

namespace fmt {

template <>
struct formatter<poac::core::builder::compiler::lang::Lang> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline auto
  format(poac::core::builder::compiler::lang::Lang l, FormatContext& ctx) {
    return format_to(
        ctx.out(), "{}", poac::core::builder::compiler::lang::to_string(l)
    );
  }
};

} // namespace fmt

#endif // POAC_CORE_BUILDER_COMPILER_LANG_HPP_
