#ifndef POAC_UTIL_VALIDATOR_HPP_
#define POAC_UTIL_VALIDATOR_HPP_

// external
#include <toml.hpp>

// internal
#include "poac/config.hpp"
#include "poac/data/manifest.hpp"
#include "poac/poac.hpp"

namespace poac::util::validator {

[[nodiscard]] Result<Path, String> required_config_exists();

[[nodiscard]] Result<void, String> can_create_directory(const Path& p);

constexpr bool is_digit(const char c) noexcept { return '0' <= c && c <= '9'; }

constexpr bool is_alphabet(const char c) noexcept {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

constexpr bool is_alpha_numeric(const char c) noexcept {
  return is_digit(c) || is_alphabet(c);
}

[[nodiscard]] Result<void, String> two_or_more_symbols(StringRef s) noexcept;

[[nodiscard]] Result<void, String> start_with_symbol(StringRef s) noexcept;

[[nodiscard]] Result<void, String> end_with_symbol(StringRef s) noexcept;

[[nodiscard]] Result<void, String> invalid_characters_impl(StringRef s
) noexcept;

[[nodiscard]] Result<void, String> invalid_characters(StringRef s) noexcept;

[[nodiscard]] Result<void, String> using_keywords(StringRef s);

[[nodiscard]] Result<void, String> one_char(StringRef s);

[[nodiscard]] Result<void, String> valid_package_name(StringRef s);

[[nodiscard]] Result<void, String> valid_version(StringRef s);

[[nodiscard]] Result<void, String> valid_athr(StringRef s);

[[nodiscard]] Result<void, String> valid_authors(const Vec<String>& authors);

[[nodiscard]] Result<void, String> valid_edition(const i32& edition);

[[nodiscard]] Result<void, String> valid_license(StringRef license);

[[nodiscard]] Result<void, String> valid_repository(StringRef repo);

[[nodiscard]] Result<void, String> valid_description(StringRef desc);

[[nodiscard]] Result<data::manifest::PartialPackage, String>
valid_manifest(const toml::value& manifest);

[[nodiscard]] Result<Option<String>, String>
valid_profile(const Option<String>& profile, Option<bool> release);

} // namespace poac::util::validator

#endif // POAC_UTIL_VALIDATOR_HPP_
