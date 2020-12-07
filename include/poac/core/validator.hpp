#ifndef POAC_CORE_VALIDATOR_HPP
#define POAC_CORE_VALIDATOR_HPP

// std
#include <filesystem>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>

namespace poac::core::validator {
    [[nodiscard]] mitama::result<void, std::string>
    require_config_exists(
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
                config_path
            )
        );
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
    use_valid_characters(std::string_view s) noexcept {
        for (const auto& c : s) {
            if (!is_alpha_numeric(c) && c != '_' && c != '-' && c != '/') {
                return mitama::failure(
                    "Invalid name.\n"
                    "It is prohibited to use a character string\n"
                    " that does not match ^([a-z|\\d|_|\\-|\\/]*)$\n"
                    " in the project name."
                );
            }
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    not_using_keywords(std::string_view s) {
        // Ban keywords
        // https://en.cppreference.com/w/cpp/keyword
        std::vector<std::string_view> blacklist{
            "alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel", "atomic_commit", "atomic_noexcept",
            "auto", "bitand", "bitor", "bool", "break", "case", "catch", "char", "char8_t", "char16_t", "char32_t",
            "class", "compl", "concept", "const", "consteval", "constexpr", "const_cast", "continue", "co_await",
            "co_return", "co_yield", "decltype", "default", "delete", "do", "double", "dynamic_cast", "else", "enum",
            "explicit", "export", "extern", "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
            "mutable", "namespace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private",
            "protected", "public", "reflexpr", "register", "reinterpret_cast", "requires", "return", "short", "signed",
            "sizeof", "static", "static_assert", "static_cast", "struct", "switch", "synchronized", "template", "this",
            "thread_local", "throw", "true", "try", "typedef", "typeid", "typename", "union", "unsigned", "using",
            "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq",
        };
        if (std::find(blacklist.begin(), blacklist.end(), s) != blacklist.end()) {
            return mitama::failure(
                fmt::format(
                    "`{}` is a keyword, so it cannot be used as a package name", s
                )
            );
        }
        return mitama::success();
    }

    [[nodiscard]] mitama::result<void, std::string>
    valid_package_name(std::string_view s) {
        MITAMA_TRY(use_valid_characters(s));
        MITAMA_TRY(not_using_keywords(s));
        return mitama::success();
    }
} // end namespace

#endif // POAC_CORE_VALIDATOR_HPP
