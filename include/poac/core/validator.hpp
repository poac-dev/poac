#ifndef POAC_CORE_VALIDATOR_HPP_
#define POAC_CORE_VALIDATOR_HPP_

// std
#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

// external
#include <toml.hpp>

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
can_create_directory(const fs::path& p) {
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

    constexpr bool
    is_alphabet(const char& c) noexcept {
        return ('A' <= c && c <= 'Z')
               || ('a' <= c && c <= 'z');
    }

    constexpr bool
    is_alpha_numeric(const char& c) noexcept {
        return is_digit(c) || is_alphabet(c);
    }

    [[nodiscard]] mitama::result<void, std::string>
    two_or_more_symbols(std::string_view s) noexcept {
        const std::size_t slashes = std::count(s.begin(), s.end(), '/');
        if (slashes > 1) {
            return mitama::failure(
                "Invalid package name.\n"
                "It is prohibited to use a character string\n"
                " that is two or more `/`."
            );
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    start_with_symbol(std::string_view s) noexcept {
        if (s[0] == '_' || s[0] == '-' || s[0] == '/') {
            return mitama::failure(
                "Invalid package name.\n"
                "It is prohibited to use a character string\n"
                " that starts with `_`, `-`, and `/`."
            );
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    end_with_symbol(std::string_view s) noexcept {
        const char last = s[s.size() - 1];
        if (last == '_' || last == '-' || last == '/') {
            return mitama::failure(
                "Invalid package name.\n"
                "It is prohibited to use a character string\n"
                " that ends with `_`, `-`, and `/`."
            );
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    invalid_characters_impl(std::string_view s) noexcept {
        for (const auto& c : s) {
            if (!is_alpha_numeric(c) && c != '_' && c != '-' && c != '/') {
                return mitama::failure(
                    "Invalid package name.\n"
                    "It is prohibited to use a character string\n"
                    " that does not match ^([a-z|\\d|_|\\-|\\/]*)$."
                );
            }
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    invalid_characters(std::string_view s) noexcept {
        MITAMA_TRY(invalid_characters_impl(s));
        MITAMA_TRY(start_with_symbol(s));
        MITAMA_TRY(end_with_symbol(s));
        MITAMA_TRY(two_or_more_symbols(s));
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    using_keywords(std::string_view s) {
        // Ban keywords
        // https://en.cppreference.com/w/cpp/keyword
        std::vector<std::string_view> blacklist{
            "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
            "atomic_commit", "atomic_noexcept", "auto", "bitand", "bitor",
            "bool", "break", "case", "catch", "char", "char8_t", "char16_t",
            "char32_t", "class", "compl", "concept", "const", "consteval",
            "constexpr", "const_cast", "continue", "co_await", "co_return",
            "co_yield", "decltype", "default", "delete", "do", "double",
            "dynamic_cast", "else", "enum", "explicit", "export", "extern",
            "false", "float", "for", "friend", "goto", "if", "inline", "int",
            "long", "mutable", "namespace", "new", "noexcept", "not", "not_eq",
            "nullptr", "operator", "or", "or_eq", "private", "protected",
            "public", "reflexpr", "register", "reinterpret_cast", "requires",
            "return", "short", "signed", "sizeof", "static", "static_assert",
            "static_cast", "struct", "switch", "synchronized", "template",
            "this", "thread_local", "throw", "true", "try", "typedef", "typeid",
            "typename", "union", "unsigned", "using", "virtual", "void",
            "volatile", "wchar_t", "while", "xor", "xor_eq",
        };
        if (std::find(blacklist.begin(), blacklist.end(), s) != blacklist.end()) {
            return mitama::failure(fmt::format(
                "`{}` is a keyword, so it cannot be used as a package name",
                std::string(s)
            ));
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_package_name(std::string_view s) {
        MITAMA_TRY(invalid_characters(s));
        MITAMA_TRY(using_keywords(s));
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_version(std::string_view s) {
        try {
            semver::parse(s);
        } catch (const semver::exception& e) {
            return mitama::failure(fmt::format(
                "version `{}` is not compliant with the Semantic Versioning notation.\n"
                "For more information, please go to: https://semver.org/",
                std::string(s)
            ));
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_athr(std::string_view s) {
        // TODO: Email address parser
        if (auto pos = s.find('<'); pos != std::string_view::npos) {
            if (pos = s.find('@', pos + 1); pos != std::string_view::npos) {
                if (pos = s.find('>', pos + 1); pos != std::string_view::npos) {
                    if (s.ends_with('>')) {
                        return mitama::success();
                    }
                }
            }
        }
        return mitama::failure(fmt::format(
            "author `{}` is written in invalid style. It should be like:\n"
            "  `Your Name <your-public@email.address>`",
            std::string(s)
        ));
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_authors(const std::vector<std::string>& authors) {
        if (authors.empty()) {
            return mitama::failure("key `authors` cannot be empty.");
        }
        for (std::string_view a : authors) {
            MITAMA_TRY(valid_athr(a));
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_edition(const std::int32_t& edition) {
        switch (edition) {
            case 1998:
            case 2003:
            case 2011:
            case 2014:
            case 2017:
            case 2020:
            case 2023:
                return mitama::success();
            default:
                return mitama::failure(fmt::format(
                    "`{}` cannot be used as an `edition` key. Possible values are:\n"
                    "  1998, 2003, 2011, 2014, 2017, 2020, and 2023",
                    edition
                ));
        }
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_license(std::string_view license) {
        // This list is from https://choosealicense.com/licenses
        if (license == "AGPL-3.0" ||
            license == "GPL-3.0" ||
            license == "LGPL-3.0" ||
            license == "MPL-2.0" ||
            license == "Apache-2.0" ||
            license == "MIT" ||
            license == "BSL-1.0" ||
            license == "Unlicense") {
            return mitama::success();
        }
        return mitama::failure(fmt::format(
            "`{}` cannot be used as `license` key. Possible values are:\n"
            "  `AGPL-3.0`, `GPL-3.0`, `LGPL-3.0`, `MPL-2.0`, `Apache-2.0`, "
            "`MIT`, `BSL-1.0`, and `Unlicense`",
            std::string(license)
        ));
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_repository(std::string_view repo) {
        // Can be parsed? it should be:
        // https://github.com/org/repo/tree/tag
        if (repo.starts_with("https://github.com/")) {
            // org/
            // 19: size of `https://github.com/`
            if (auto pos = repo.find('/', 19); pos != std::string_view::npos) {
                // repo/
                if (pos = repo.find('/', pos + 1); pos != std::string_view::npos) {
                    // tree/
                    if (repo.substr(pos + 1, 5) == "tree/") {
                        // repo/tree/
                        //     ^---->^
                        pos += 5;
                        // tag
                        if (repo.size() - 1 > pos) {
                            // -- tag found case
                            // tree/tag `8 (size)`
                            //      ^   `5 (pos)`

                            // -- no tag case
                            // tree/  `5 (size)`
                            //      ^ `5 (pos)`
                            return mitama::success();
                        }
                    }
                }
            }
        }
        return mitama::failure(fmt::format(
            "`{}` is invalid form for the `repository` key. It should be like:\n"
            "  `https://github.com/org/repo/tree/tag`",
            std::string(repo)
        ));
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_description(std::string_view desc) {
        const size_t size = desc.size();
        if (size < 10) {
            return mitama::failure(
                "the `description` key cannot be smaller than 10 characters long.\n"
                "The `description` must be descriptive."
            );
        } else if (size > 180) {
            return mitama::failure(
                "the `description` key cannot exceed 180 characters long.\n"
                "Please specify it more briefly."
            );
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<data::manifest::PartialPackage, std::string>
    valid_manifest(const toml::value& manifest) {
        const auto package = toml::find<data::manifest::PartialPackage>(manifest, "package");
        MITAMA_TRY(valid_package_name(package.name));
        MITAMA_TRY(valid_version(package.version));
        MITAMA_TRY(valid_authors(package.authors));
        MITAMA_TRY(valid_edition(package.edition));
        MITAMA_TRY(valid_license(package.license));
        MITAMA_TRY(valid_repository(package.repository));
        MITAMA_TRY(valid_description(package.description));
        return mitama::success(package);
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

#endif // POAC_CORE_VALIDATOR_HPP_
