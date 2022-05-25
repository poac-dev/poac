// internal
#include "poac/core/builder/compiler/lang/lang.hpp"

namespace poac::core::builder::compiler::lang {

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

} // namespace poac::core::builder::compiler::lang
