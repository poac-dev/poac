#ifndef POAC_CORE_NAME_HPP
#define POAC_CORE_NAME_HPP

#include <algorithm>
#include <string>
#include <string_view>

#include <poac/core/except.hpp>

namespace poac::core::name {
    // Config name: poacpm/poac
    // Cache name: poacpm-poac-version
    // Current name: poacpm-poac

    // poacpm/poac, 0.1.0 -> poacpm-poac-0.1.0
    std::string
    to_cache(std::string name, const std::string& version) {
        std::replace(name.begin(), name.end(), '/', '-');
        return name + "-" + version;
    }

    // poacpm/poac -> poacpm-poac
    std::string
    to_current(std::string name) {
        std::replace(name.begin(), name.end(), '/', '-');
        return name;
    }

    constexpr bool
    is_digit(const char& c) noexcept {
        return '0' <= c && c <= '9';
    }

    constexpr bool
    is_alphabet(const char& c) noexcept {
        return ('A' <= c && c <= 'Z')
            || ('a' <= c && c <= 'z');
    }

    constexpr bool
    is_alpha_numeric(const char& c) noexcept {
        return is_digit(c) || is_alphabet(c);
    }

    [[nodiscard]] std::optional<core::except::Error>
    validate_package_name(std::string_view s) {
        for (const auto& c : s) {
            if (!is_alpha_numeric(c) && c != '_' && c != '-' && c != '/') {
                return except::Error::General{
                        "Invalid name.\n"
                        "It is prohibited to use a character string\n"
                        " that does not match ^([a-z|\\d|_|\\-|\\/]*)$\n"
                        " in the project name."
                };
            }
        }
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_CORE_NAME_HPP
