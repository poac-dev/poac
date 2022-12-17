// internal
#include "poac/core/builder/compiler/lang.hpp"

namespace poac::core::builder::compiler::lang {

Fn to_string(Lang lang)->String {
  switch (lang) {
    case Lang::c:
      return "C";
    case Lang::cxx:
      return "C++";
    default:
      unreachable();
  }
}

} // namespace poac::core::builder::compiler::lang
