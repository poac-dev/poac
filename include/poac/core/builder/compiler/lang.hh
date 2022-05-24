#ifndef POAC_CORE_BUILDER_COMPILER_LANG_HPP_
#define POAC_CORE_BUILDER_COMPILER_LANG_HPP_

// internal
#include "poac/poac.hh"

#include <poac/util/cfg.hh> // compiler

namespace poac::core::builder::compiler::lang {

enum class Lang {
  c,
  cxx,
};

String
to_string(Lang lang) {
  switch (lang) {
    case Lang::c:
      return "C";
    case Lang::cxx:
      return "C++";
    default:
      unreachable();
  }
}

std::ostream&
operator<<(std::ostream& os, Lang lang) {
  return (os << to_string(lang));
}

} // namespace poac::core::builder::compiler::lang

#endif // POAC_CORE_BUILDER_COMPILER_LANG_HPP_
