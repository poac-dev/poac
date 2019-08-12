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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_gray(const std::basic_string<CharT>& str) noexcept
    {
        return gray<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_red(const std::basic_string<CharT>& str) noexcept
    {
        return red<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_green(const std::basic_string<CharT>& str) noexcept
    {
        return green<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_yellow(const std::basic_string<CharT>& str) noexcept
    {
        return yellow<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_blue(const std::basic_string<CharT>& str) noexcept
    {
        return blue<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_magenta(const std::basic_string<CharT>& str) noexcept
    {
        return magenta<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_cyan(const std::basic_string<CharT>& str) noexcept
    {
        return cyan<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_white(const std::basic_string<CharT>& str) noexcept
    {
        return white<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    //
    // Background manipulators
    //
    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_gray<CharT>.size() + N + reset<CharT>.size()>
    to_on_gray() noexcept
    {
        return on_gray<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_gray<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_on_gray(const CharT(&arr)[N]) noexcept
    {
        return on_gray<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_gray(const CharT* str, std::size_t len) noexcept
    {
        return on_gray<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_gray(const std::basic_string<CharT>& str) noexcept
    {
        return on_gray<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_red<CharT>.size() + N + reset<CharT>.size()>
    to_on_red() noexcept
    {
        return on_red<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_red<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_on_red(const CharT(&arr)[N]) noexcept
    {
        return on_red<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_red(const CharT* str, std::size_t len) noexcept
    {
        return on_red<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_red(const std::basic_string<CharT>& str) noexcept
    {
        return on_red<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_green<CharT>.size() + N + reset<CharT>.size()>
    to_on_green() noexcept
    {
        return on_green<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_green<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_on_green(const CharT(&arr)[N]) noexcept
    {
        return on_green<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_green(const CharT* str, std::size_t len) noexcept
    {
        return on_green<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_green(const std::basic_string<CharT>& str) noexcept
    {
        return on_green<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_yellow<CharT>.size() + N + reset<CharT>.size()>
    to_on_yellow() noexcept
    {
        return on_yellow<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_yellow<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_on_yellow(const CharT(&arr)[N]) noexcept
    {
        return on_yellow<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_yellow(const CharT* str, std::size_t len) noexcept
    {
        return on_yellow<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_yellow(const std::basic_string<CharT>& str) noexcept
    {
        return on_yellow<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_blue<CharT>.size() + N + reset<CharT>.size()>
    to_on_blue() noexcept
    {
        return on_blue<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_blue<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_on_blue(const CharT(&arr)[N]) noexcept
    {
        return on_blue<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_blue(const CharT* str, std::size_t len) noexcept
    {
        return on_blue<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_blue(const std::basic_string<CharT>& str) noexcept
    {
        return on_blue<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_magenta<CharT>.size() + N + reset<CharT>.size()>
    to_on_magenta() noexcept
    {
        return on_magenta<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_magenta<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_on_magenta(const CharT(&arr)[N]) noexcept
    {
        return on_magenta<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_magenta(const CharT* str, std::size_t len) noexcept
    {
        return on_magenta<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_magenta(const std::basic_string<CharT>& str) noexcept
    {
        return on_magenta<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_cyan<CharT>.size() + N + reset<CharT>.size()>
    to_on_cyan() noexcept
    {
        return on_cyan<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_cyan<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_on_cyan(const CharT(&arr)[N]) noexcept
    {
        return on_cyan<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_cyan(const CharT* str, std::size_t len) noexcept
    {
        return on_cyan<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_cyan(const std::basic_string<CharT>& str) noexcept
    {
        return on_cyan<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_white<CharT>.size() + N + reset<CharT>.size()>
    to_on_white() noexcept
    {
        return on_white<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_white<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_on_white(const CharT(&arr)[N]) noexcept
    {
        return on_white<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_white(const CharT* str, std::size_t len) noexcept
    {
        return on_white<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_white(const std::basic_string<CharT>& str) noexcept
    {
        return on_white<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_bold(const std::basic_string<CharT>& str) noexcept
    {
        return bold<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, dark<CharT>.size() + N + reset<CharT>.size()>
    to_dark() noexcept
    {
        return dark<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, dark<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_dark(const CharT(&arr)[N]) noexcept
    {
        return dark<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_dark(const CharT* str, std::size_t len) noexcept
    {
        return dark<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_dark(const std::basic_string<CharT>& str) noexcept
    {
        return dark<CharT>.to_string() + str + reset<CharT>.to_string();
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
    template <typename CharT>
    inline std::basic_string<CharT>
    to_underline(const std::basic_string<CharT>& str) noexcept
    {
        return underline<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, blink<CharT>.size() + N + reset<CharT>.size()>
    to_blink() noexcept
    {
        return blink<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, blink<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_blink(const CharT(&arr)[N]) noexcept
    {
        return blink<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_blink(const CharT* str, std::size_t len) noexcept
    {
        return blink<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_blink(const std::basic_string<CharT>& str) noexcept
    {
        return blink<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, reverse<CharT>.size() + N + reset<CharT>.size()>
    to_reverse() noexcept
    {
        return reverse<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, reverse<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_reverse(const CharT(&arr)[N]) noexcept
    {
        return reverse<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_reverse(const CharT* str, std::size_t len) noexcept
    {
        return reverse<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_reverse(const std::basic_string<CharT>& str) noexcept
    {
        return reverse<CharT>.to_string() + str + reset<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, concealed<CharT>.size() + N + reset<CharT>.size()>
    to_concealed() noexcept
    {
        return concealed<CharT> + basic_string<CharT, N>({Str...}) + reset<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, concealed<CharT>.size() + (N - 1) + reset<CharT>.size()>
    to_concealed(const CharT(&arr)[N]) noexcept
    {
        return concealed<CharT> + basic_string<CharT, N - 1>(arr) + reset<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_concealed(const CharT* str, std::size_t len) noexcept
    {
        return concealed<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_concealed(const std::basic_string<CharT>& str) noexcept
    {
        return concealed<CharT>.to_string() + str + reset<CharT>.to_string();
    }
}

#endif	// !TERMCOLOR2_TO_COLOR_HPP
