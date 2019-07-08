#ifndef TERMCOLOR2_TO_COLOR_HPP
#define TERMCOLOR2_TO_COLOR_HPP

#include <cstddef> // std::size_t
#include <string> // std::basic_string

#include <poac/util/termcolor2/presets.hpp>
#include <poac/util/termcolor2/string.hpp>

namespace termcolor2 {
    //
    // Foreground manipulators
    //
    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, gray<CharT>.size() + N + reset<CharT>.size()>
    to_gray() noexcept
    {
        return gray<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, gray<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_gray(const CharT(&arr)[N]) noexcept
    {
        return gray<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_gray(const CharT* str, std::size_t len) noexcept
    {
        return gray<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, red<CharT>.size() + N + reset<CharT>.size()>
    to_red() noexcept
    {
        return red<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, red<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_red(const CharT(&arr)[N]) noexcept
    {
        return red<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_red(const CharT* str, std::size_t len) noexcept
    {
        return red<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, green<CharT>.size() + N + reset<CharT>.size()>
    to_green() noexcept
    {
        return green<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, green<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_green(const CharT(&arr)[N]) noexcept
    {
        return green<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_green(const CharT* str, std::size_t len) noexcept
    {
        return green<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, yellow<CharT>.size() + N + reset<CharT>.size()>
    to_yellow() noexcept
    {
        return yellow<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, yellow<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_yellow(const CharT(&arr)[N]) noexcept
    {
        return yellow<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_yellow(const CharT* str, std::size_t len) noexcept
    {
        return yellow<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, blue<CharT>.size() + N + reset<CharT>.size()>
    to_blue() noexcept
    {
        return blue<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, blue<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_blue(const CharT(&arr)[N]) noexcept
    {
        return blue<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_blue(const CharT* str, std::size_t len) noexcept
    {
        return blue<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, magenta<CharT>.size() + N + reset<CharT>.size()>
    to_magenta() noexcept
    {
        return magenta<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, magenta<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_magenta(const CharT(&arr)[N]) noexcept
    {
        return magenta<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_magenta(const CharT* str, std::size_t len) noexcept
    {
        return magenta<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, cyan<CharT>.size() + N + reset<CharT>.size()>
    to_cyan() noexcept
    {
        return cyan<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, cyan<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_cyan(const CharT(&arr)[N]) noexcept
    {
        return cyan<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_cyan(const CharT* str, std::size_t len) noexcept
    {
        return cyan<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, white<CharT>.size() + N + reset<CharT>.size()>
    to_white() noexcept
    {
        return white<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, white<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_white(const CharT(&arr)[N]) noexcept
    {
        return white<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_white(const CharT* str, std::size_t len) noexcept
    {
        return white<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    //
    // Attribute manipulators
    //
    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, bold<CharT>.size() + N + reset<CharT>.size()>
    to_bold() noexcept
    {
        return bold<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, bold<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_bold(const CharT(&arr)[N]) noexcept
    {
        return bold<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_bold(const CharT* str, std::size_t len) noexcept
    {
        return bold<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, underline<CharT>.size() + N + reset<CharT>.size()>
    to_underline() noexcept
    {
        return underline<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, underline<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_underline(const CharT(&arr)[N]) noexcept
    {
        return underline<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_underline(const CharT* str, std::size_t len) noexcept
    {
        return underline<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
}

#endif	// !TERMCOLOR2_TO_COLOR_HPP
