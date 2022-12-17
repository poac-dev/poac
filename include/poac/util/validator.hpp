#pragma once

// external
#include <toml.hpp>

// internal
#include "poac/config.hpp"
#include "poac/data/manifest.hpp"
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::util::validator {

[[nodiscard]] Fn required_config_exists(bool find_parents = true)
    ->Result<Path, String>;

[[nodiscard]] Fn can_create_directory(const Path& p)->Result<void, String>;

constexpr Fn is_digit(const char c) noexcept -> bool {
  return '0' <= c && c <= '9';
}

constexpr Fn is_alphabet(const char c) noexcept -> bool {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

constexpr Fn is_alpha_numeric(const char c) noexcept -> bool {
  return is_digit(c) || is_alphabet(c);
}

[[nodiscard]] Fn two_or_more_symbols(StringRef s) noexcept
    -> Result<void, String>;

[[nodiscard]] Fn start_with_symbol(StringRef s) noexcept
    -> Result<void, String>;

[[nodiscard]] Fn end_with_symbol(StringRef s) noexcept -> Result<void, String>;

[[nodiscard]] Fn invalid_characters_impl(StringRef s) noexcept
    -> Result<void, String>;

[[nodiscard]] Fn invalid_characters(StringRef s) noexcept
    -> Result<void, String>;

[[nodiscard]] Fn using_keywords(StringRef s)->Result<void, String>;

[[nodiscard]] Fn one_char(StringRef s)->Result<void, String>;

[[nodiscard]] Fn valid_package_name(StringRef s)->Result<void, String>;

[[nodiscard]] Fn valid_version(StringRef s)->Result<void, String>;

[[nodiscard]] Fn valid_athr(StringRef s)->Result<void, String>;

[[nodiscard]] Fn valid_authors(const Vec<String>& authors)
    ->Result<void, String>;

[[nodiscard]] Fn valid_edition(const i32& edition)->Result<void, String>;

[[nodiscard]] Fn valid_license(StringRef license)->Result<void, String>;

[[nodiscard]] Fn valid_repository(StringRef repo)->Result<void, String>;

[[nodiscard]] Fn valid_description(StringRef desc)->Result<void, String>;

[[nodiscard]] Fn valid_manifest(const toml::value& manifest)
    ->Result<data::manifest::PartialPackage, String>;

[[nodiscard]] Fn
valid_profile(const Option<String>& profile, Option<bool> release)
    ->Result<Option<String>, String>;

} // namespace poac::util::validator
