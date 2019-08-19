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
    constexpr basic_string<CharT, gray_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_gray() noexcept
    {
        return gray_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, gray_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_gray(const CharT(&arr)[N]) noexcept
    {
        return gray_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_gray(const CharT* str, std::size_t len) noexcept
    {
        return gray_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_gray(const std::basic_string<CharT>& str) noexcept
    {
        return gray_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, red_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_red() noexcept
    {
        return red_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, red_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_red(const CharT(&arr)[N]) noexcept
    {
        return red_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_red(const CharT* str, std::size_t len) noexcept
    {
        return red_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_red(const std::basic_string<CharT>& str) noexcept
    {
        return red_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, green_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_green() noexcept
    {
        return green_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, green_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_green(const CharT(&arr)[N]) noexcept
    {
        return green_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_green(const CharT* str, std::size_t len) noexcept
    {
        return green_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_green(const std::basic_string<CharT>& str) noexcept
    {
        return green_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, yellow_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_yellow() noexcept
    {
        return yellow_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, yellow_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_yellow(const CharT(&arr)[N]) noexcept
    {
        return yellow_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_yellow(const CharT* str, std::size_t len) noexcept
    {
        return yellow_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_yellow(const std::basic_string<CharT>& str) noexcept
    {
        return yellow_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, blue_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_blue() noexcept
    {
        return blue_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, blue_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_blue(const CharT(&arr)[N]) noexcept
    {
        return blue_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_blue(const CharT* str, std::size_t len) noexcept
    {
        return blue_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_blue(const std::basic_string<CharT>& str) noexcept
    {
        return blue_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, magenta_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_magenta() noexcept
    {
        return magenta_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, magenta_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_magenta(const CharT(&arr)[N]) noexcept
    {
        return magenta_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_magenta(const CharT* str, std::size_t len) noexcept
    {
        return magenta_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_magenta(const std::basic_string<CharT>& str) noexcept
    {
        return magenta_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, cyan_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_cyan() noexcept
    {
        return cyan_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, cyan_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_cyan(const CharT(&arr)[N]) noexcept
    {
        return cyan_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_cyan(const CharT* str, std::size_t len) noexcept
    {
        return cyan_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_cyan(const std::basic_string<CharT>& str) noexcept
    {
        return cyan_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, white_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_white() noexcept
    {
        return white_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, white_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_white(const CharT(&arr)[N]) noexcept
    {
        return white_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_white(const CharT* str, std::size_t len) noexcept
    {
        return white_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_white(const std::basic_string<CharT>& str) noexcept
    {
        return white_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    //
    // Background manipulators
    //
    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_gray_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_on_gray() noexcept
    {
        return on_gray_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_gray_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_on_gray(const CharT(&arr)[N]) noexcept
    {
        return on_gray_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_gray(const CharT* str, std::size_t len) noexcept
    {
        return on_gray_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_gray(const std::basic_string<CharT>& str) noexcept
    {
        return on_gray_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_red_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_on_red() noexcept
    {
        return on_red_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_red_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_on_red(const CharT(&arr)[N]) noexcept
    {
        return on_red_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_red(const CharT* str, std::size_t len) noexcept
    {
        return on_red_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_red(const std::basic_string<CharT>& str) noexcept
    {
        return on_red_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_green_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_on_green() noexcept
    {
        return on_green_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_green_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_on_green(const CharT(&arr)[N]) noexcept
    {
        return on_green_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_green(const CharT* str, std::size_t len) noexcept
    {
        return on_green_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_green(const std::basic_string<CharT>& str) noexcept
    {
        return on_green_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_yellow_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_on_yellow() noexcept
    {
        return on_yellow_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_yellow_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_on_yellow(const CharT(&arr)[N]) noexcept
    {
        return on_yellow_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_yellow(const CharT* str, std::size_t len) noexcept
    {
        return on_yellow_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_yellow(const std::basic_string<CharT>& str) noexcept
    {
        return on_yellow_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_blue_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_on_blue() noexcept
    {
        return on_blue_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_blue_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_on_blue(const CharT(&arr)[N]) noexcept
    {
        return on_blue_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_blue(const CharT* str, std::size_t len) noexcept
    {
        return on_blue_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_blue(const std::basic_string<CharT>& str) noexcept
    {
        return on_blue_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_magenta_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_on_magenta() noexcept
    {
        return on_magenta_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_magenta_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_on_magenta(const CharT(&arr)[N]) noexcept
    {
        return on_magenta_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_magenta(const CharT* str, std::size_t len) noexcept
    {
        return on_magenta_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_magenta(const std::basic_string<CharT>& str) noexcept
    {
        return on_magenta_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_cyan_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_on_cyan() noexcept
    {
        return on_cyan_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_cyan_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_on_cyan(const CharT(&arr)[N]) noexcept
    {
        return on_cyan_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_cyan(const CharT* str, std::size_t len) noexcept
    {
        return on_cyan_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_cyan(const std::basic_string<CharT>& str) noexcept
    {
        return on_cyan_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, on_white_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_on_white() noexcept
    {
        return on_white_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, on_white_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_on_white(const CharT(&arr)[N]) noexcept
    {
        return on_white_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_white(const CharT* str, std::size_t len) noexcept
    {
        return on_white_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_on_white(const std::basic_string<CharT>& str) noexcept
    {
        return on_white_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    //
    // Attribute manipulators
    //
    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, bold_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_bold() noexcept
    {
        return bold_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, bold_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_bold(const CharT(&arr)[N]) noexcept
    {
        return bold_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_bold(const CharT* str, std::size_t len) noexcept
    {
        return bold_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_bold(const std::basic_string<CharT>& str) noexcept
    {
        return bold_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, dark_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_dark() noexcept
    {
        return dark_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, dark_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_dark(const CharT(&arr)[N]) noexcept
    {
        return dark_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_dark(const CharT* str, std::size_t len) noexcept
    {
        return dark_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_dark(const std::basic_string<CharT>& str) noexcept
    {
        return dark_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, underline_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_underline() noexcept
    {
        return underline_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, underline_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_underline(const CharT(&arr)[N]) noexcept
    {
        return underline_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_underline(const CharT* str, std::size_t len) noexcept
    {
        return underline_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_underline(const std::basic_string<CharT>& str) noexcept
    {
        return underline_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, blink_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_blink() noexcept
    {
        return blink_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, blink_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_blink(const CharT(&arr)[N]) noexcept
    {
        return blink_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_blink(const CharT* str, std::size_t len) noexcept
    {
        return blink_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_blink(const std::basic_string<CharT>& str) noexcept
    {
        return blink_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, reverse_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_reverse() noexcept
    {
        return reverse_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, reverse_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_reverse(const CharT(&arr)[N]) noexcept
    {
        return reverse_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_reverse(const CharT* str, std::size_t len) noexcept
    {
        return reverse_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_reverse(const std::basic_string<CharT>& str) noexcept
    {
        return reverse_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }

    template <typename CharT, std::size_t N, CharT... Str>
    constexpr basic_string<CharT, concealed_v<CharT>.size() + N + reset_v<CharT>.size()>
    to_concealed() noexcept
    {
        return concealed_v<CharT> + basic_string<CharT, N>({Str...}) + reset_v<CharT>;
    }
    template <typename CharT, std::size_t N>
    constexpr basic_string<CharT, concealed_v<CharT>.size() + (N - 1) + reset_v<CharT>.size()>
    to_concealed(const CharT(&arr)[N]) noexcept
    {
        return concealed_v<CharT> + basic_string<CharT, N - 1>(arr) + reset_v<CharT>;
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_concealed(const CharT* str, std::size_t len) noexcept
    {
        return concealed_v<CharT>.to_string() + std::basic_string<CharT>(str, len) + reset_v<CharT>.to_string();
    }
    template <typename CharT>
    inline std::basic_string<CharT>
    to_concealed(const std::basic_string<CharT>& str) noexcept
    {
        return concealed_v<CharT>.to_string() + str + reset_v<CharT>.to_string();
    }
}

#endif	// !TERMCOLOR2_TO_COLOR_HPP
