#ifndef POAC_UTIL_HPP
#define POAC_UTIL_HPP

#include <poac/util/cfg.hpp>
#include <poac/util/clap/clap.hpp>
#include <poac/util/git2-cpp/git2.hpp>
#include <poac/util/misc.hpp>
#include <poac/util/pretty.hpp>
#include <poac/util/semver/semver.hpp>
#include <poac/util/shell.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>
#include <poac/util/types.hpp>
#include <poac/util/vcs.hpp>

#include <cstdlib>
#include <string_view>

namespace poac::util {

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

/// Check the base requirements for a package name.
///
/// This can be used for other things than package names, to enforce some
/// level of sanity. Note that package names have other restrictions
/// elsewhere. `poac new` has a few restrictions, such as checking for
/// reserved names. poac.pm has even more restrictions.
void
validate_package_name(std::string_view name, std::string_view what, std::string_view help) {
    for (const auto& c : name) {
        if (!is_alpha_numeric(c) && c != '_' && c != '-') {
            throw core::except::error(
                "Invalid character `", c, "` in ", what, ": `", name, "`", help);
        }
    }
}

/// Whether or not this running in a Continuous Integration environment.
bool
is_ci() {
    return (std::getenv("CI") != nullptr) || (std::getenv("TF_BUILD") != nullptr);
}

} // namespace poac::util

#endif // !POAC_UTIL_HPP
