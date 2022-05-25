// internal
#include "poac/core/builder/compiler/lang/error.hpp"

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

} // namespace poac::core::builder::compiler::error
