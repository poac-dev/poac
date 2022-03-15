#ifndef POAC_CORE_BUILDER_COMPILER_ERROR_HPP
#define POAC_CORE_BUILDER_COMPILER_ERROR_HPP

// std
#include <cstdint> // std::int64_t
#include <string>

// external
#include <mitama/result/result.hpp>
#include <mitama/anyhow/anyhow.hpp>
#include <mitama/thiserror/thiserror.hpp>

// internal
#include <poac/core/builder/compiler/lang.hpp>
#include <poac/util/cfg.hpp> // compiler

namespace poac::core::builder::compiler::error {
    namespace anyhow = mitama::anyhow;
    namespace thiserror = mitama::thiserror;

    std::string
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
                throw std::logic_error(
                    "To access out of range of the "
                    "enumeration values is undefined behavior."
                );
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
                std::string, std::string, lang::lang_t, std::int64_t
            >;

        using FailedToGetCompilerVersion =
            error<"failed to get version of compiler `{0}`", std::string>;
    };
} // end namespace

#endif // !POAC_CORE_BUILDER_COMPILER_ERROR_HPP
