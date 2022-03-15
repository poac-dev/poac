#ifndef POAC_CORE_BUILDER_COMPILER_LANG_HPP
#define POAC_CORE_BUILDER_COMPILER_LANG_HPP

// std
#include <string>
#include <stdexcept> // std::logic_error

// internal
#include <poac/util/cfg.hpp> // compiler

namespace poac::core::builder::compiler::lang {
    enum class lang_t {
        c,
        cxx,
    };

    std::string
    to_string(lang_t lang) {
        switch (lang) {
            case lang_t::c:
                return "C";
            case lang_t::cxx:
                return "C++";
            default:
                throw std::logic_error(
                    "To access out of range of the "
                    "enumeration values is undefined behavior."
                );
        }
    }

    std::ostream&
    operator<<(std::ostream& os, lang_t lang) {
        return (os << to_string(lang));
    }
} // end namespace

#endif // !POAC_CORE_BUILDER_COMPILER_LANG_HPP
