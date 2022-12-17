// internal
#include "poac/core/builder/compiler/error.hpp"

namespace poac::core::builder::compiler::error {

Fn to_string(util::cfg::Compiler comp)->String {
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
      unreachable();
  }
}

} // namespace poac::core::builder::compiler::error
