#ifndef POAC_CORE_BUILDER_COMPILER_ERROR_HPP
#define POAC_CORE_BUILDER_COMPILER_ERROR_HPP

// internal
#include <poac/poac.hpp>
#include <poac/core/builder/compiler/lang.hpp>
#include <poac/util/cfg.hpp> // compiler

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

    class Error {
        template <thiserror::fixed_string S, class ...T>
        using error = thiserror::error<S, T...>;

    public:
        using UnsupportedLangVersion =
            error<
                "`{0}` ({1}) does not support {2} version: `{3}`",
                String, String, String, i64
            >;

        using FailedToGetCompilerVersion =
            error<"failed to get version of compiler `{0}`", String>;
    };
} // end namespace

#endif // !POAC_CORE_BUILDER_COMPILER_ERROR_HPP
