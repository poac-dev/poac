#ifndef POAC_CORE_VALIDATOR_HPP
#define POAC_CORE_VALIDATOR_HPP

// std
#include <algorithm>

// internal
#include <poac/config.hpp>
#include <poac/data/manifest.hpp>
#include <poac/poac.hpp>
#include <poac/util/semver/semver.hpp>

namespace poac::core::validator {

[[nodiscard]] Result<void, String>
required_config_exists(const fs::path& base = config::path::cur_dir) noexcept {
  const auto config_path = base / data::manifest::name;
  std::error_code ec{};
  if (fs::exists(config_path, ec)) {
    return Ok();
  }
  return Err(
      format("required config file `{}` does not exist", config_path.string())
  );
}

[[nodiscard]] Result<void, String>
can_crate_directory(const fs::path& p) {
  std::error_code ec{}; // This is to use for noexcept optimization

  const bool exists = fs::exists(p, ec);
  if (exists && !fs::is_directory(p, ec)) {
    return Err(format(
        "The `{}` directory could not be created "
        "because the same name file exists",
        p.string()
    ));
  } else if (exists && !fs::is_empty(p, ec)) {
    return Err(
        format("The `{}` directory already exists and is not empty", p.string())
    );
  }
  return Ok();
}

constexpr bool
is_digit(const char c) noexcept {
  return '0' <= c && c <= '9';
}

constexpr bool
is_alphabet(const char c) noexcept {
  return ('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z');
}

constexpr bool
is_alpha_numeric(const char c) noexcept {
  return is_digit(c) || is_alphabet(c);
}

[[nodiscard]] Result<void, String>
two_or_more_symbols(StringRef s) noexcept {
  const usize slashes = std::count(s.begin(), s.end(), '/');
  if (slashes > 1) {
    return Err(
        "Invalid package name.\n"
        "It is prohibited to use a character string\n"
        " that is two or more `/`."
    );
  }
  return Ok();
}

[[nodiscard]] Result<void, String>
start_with_symbol(StringRef s) noexcept {
  if (s[0] == '_' || s[0] == '-' || s[0] == '/') {
    return Err(
        "Invalid package name.\n"
        "It is prohibited to use a character string\n"
        " that starts with `_`, `-`, and `/`."
    );
  }
  return Ok();
}

[[nodiscard]] Result<void, String>
end_with_symbol(StringRef s) noexcept {
  const char last = s[s.size() - 1];
  if (last == '_' || last == '-' || last == '/') {
    return Err(
        "Invalid package name.\n"
        "It is prohibited to use a character string\n"
        " that ends with `_`, `-`, and `/`."
    );
  }
  return Ok();
}

[[nodiscard]] Result<void, String>
invalid_characters_impl(StringRef s) noexcept {
  for (const char c : s) {
    if (!is_alpha_numeric(c) && c != '_' && c != '-' && c != '/') {
      return Err(
          "Invalid package name.\n"
          "It is prohibited to use a character string\n"
          " that does not match ^([a-z|\\d|_|\\-|\\/]*)$."
      );
    }
  }
  return Ok();
}

[[nodiscard]] Result<void, String>
invalid_characters(StringRef s) noexcept {
  Try(invalid_characters_impl(s));
  Try(start_with_symbol(s));
  Try(end_with_symbol(s));
  Try(two_or_more_symbols(s));
  return Ok();
}

[[nodiscard]] Result<void, String>
using_keywords(StringRef s) {
  // Ban keywords
  // https://en.cppreference.com/w/cpp/keyword
  const Vec<StringRef> blacklist{
      "alignas",
      "alignof",
      "and",
      "and_eq",
      "asm",
      "atomic_cancel",
      "atomic_commit",
      "atomic_noexcept",
      "auto",
      "bitand",
      "bitor",
      "bool",
      "break",
      "case",
      "catch",
      "char",
      "char8_t",
      "char16_t",
      "char32_t",
      "class",
      "compl",
      "concept",
      "const",
      "consteval",
      "constexpr",
      "const_cast",
      "continue",
      "co_await",
      "co_return",
      "co_yield",
      "decltype",
      "default",
      "delete",
      "do",
      "double",
      "dynamic_cast",
      "else",
      "enum",
      "explicit",
      "export",
      "extern",
      "false",
      "float",
      "for",
      "friend",
      "goto",
      "if",
      "inline",
      "int",
      "long",
      "mutable",
      "namespace",
      "new",
      "noexcept",
      "not",
      "not_eq",
      "nullptr",
      "operator",
      "or",
      "or_eq",
      "private",
      "protected",
      "public",
      "reflexpr",
      "register",
      "reinterpret_cast",
      "requires",
      "return",
      "short",
      "signed",
      "sizeof",
      "static",
      "static_assert",
      "static_cast",
      "struct",
      "switch",
      "synchronized",
      "template",
      "this",
      "thread_local",
      "throw",
      "true",
      "try",
      "typedef",
      "typeid",
      "typename",
      "union",
      "unsigned",
      "using",
      "virtual",
      "void",
      "volatile",
      "wchar_t",
      "while",
      "xor",
      "xor_eq",
  };
  if (std::find(blacklist.begin(), blacklist.end(), s) != blacklist.end()) {
    return Err(format(
        "`{}` is a keyword, so it cannot be used as a package name", String(s)
    ));
  }
  return Ok();
}

[[nodiscard]] Result<void, String>
valid_package_name(StringRef s) {
  Try(invalid_characters(s));
  Try(using_keywords(s));
  return Ok();
}

[[nodiscard]] Result<void, String>
valid_version(StringRef s) {
  try {
    semver::parse(s);
  } catch (const semver::exception& e) {
    return Err(e.what());
  }
  return Ok();
}

} // namespace poac::core::validator

#endif // POAC_CORE_VALIDATOR_HPP
