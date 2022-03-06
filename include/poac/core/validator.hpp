#ifndef POAC_CORE_VALIDATOR_HPP
#define POAC_CORE_VALIDATOR_HPP

// std
#include <algorithm>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

// external
#include <fmt/ostream.h>
#include <mitama/result/result.hpp>

namespace poac::core::validator {
    [[nodiscard]] mitama::result<void, std::string>
    required_config_exists(
        const std::filesystem::path& base = std::filesystem::current_path())
    noexcept
    {
        const auto config_path = base / "poac.toml";
        std::error_code ec{};
        if (std::filesystem::exists(config_path, ec)) {
            return mitama::success();
        }
        return mitama::failure(
            fmt::format(
                "required config file `{}` does not exist",
                config_path.string()
            )
        );
    }

    [[nodiscard]] mitama::result<void, std::string>
    can_crate_directory(const std::filesystem::path& p) {
        namespace fs = std::filesystem;
        std::error_code ec{}; // This is to use for noexcept optimization

        const bool exists = fs::exists(p, ec);
        if (exists && !fs::is_directory(p, ec)) {
            return mitama::failure(
                fmt::format(
                    "The `{}` directory could not be created "
                    "because the same name file exists",
                    p.string()
                )
            );
        } else if (exists && !fs::is_empty(p, ec)) {
            return mitama::failure(
                fmt::format(
                    "The `{}` directory already exists and is not empty",
                    p.string()
                )
            );
        }
        return mitama::success();
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
            return mitama::failure(
                fmt::format(
                    "`{}` is a keyword, so it cannot be used as a package name",
                    std::string(s)
                )
            );
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_package_name(std::string_view s) {
        MITAMA_TRY(invalid_characters(s));
        MITAMA_TRY(using_keywords(s));
        return mitama::success();
    }
} // end namespace

#endif // POAC_CORE_VALIDATOR_HPP
