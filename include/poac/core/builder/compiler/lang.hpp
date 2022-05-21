#ifndef POAC_CORE_BUILDER_COMPILER_LANG_HPP
#define POAC_CORE_BUILDER_COMPILER_LANG_HPP

// internal
#include <poac/poac.hpp>
#include <poac/util/cfg.hpp> // compiler

namespace poac::core::builder::compiler::lang {
    enum class lang_t {
        c,
        cxx,
    };

    String
    to_string(lang_t lang) {
        switch (lang) {
            case lang_t::c:
                return "C";
            case lang_t::cxx:
                return "C++";
            default:
                unreachable();
        }
    }

    std::ostream&
    operator<<(std::ostream& os, lang_t lang) {
        return (os << to_string(lang));
    }
} // end namespace

#endif // !POAC_CORE_BUILDER_COMPILER_LANG_HPP
