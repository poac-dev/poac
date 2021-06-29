#ifndef TERMCOLOR2_TO_COLOR_HPP
#define TERMCOLOR2_TO_COLOR_HPP

#include <string> // std::basic_string
#include <string_view> // std::basic_string_view
#include <poac/util/termcolor2/presets.hpp>

namespace termcolor2 {
    //
    // Foreground manipulators
    //
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_gray(const CharT* str)
    {
        return gray_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_gray(const CharT* str, std::size_t len)
    {
        return gray_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_gray(const std::basic_string<CharT>& str)
    {
        return gray_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_gray(const std::basic_string_view<CharT> str)
    {
        return gray_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_red(const CharT* str)
    {
        return red_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_red(const CharT* str, std::size_t len)
    {
        return red_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_red(const std::basic_string<CharT>& str)
    {
        return red_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_red(const std::basic_string_view<CharT> str)
    {
        return red_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_green(const CharT* str)
    {
        return green_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_green(const CharT* str, std::size_t len)
    {
        return green_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_green(const std::basic_string<CharT>& str)
    {
        return green_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_green(const std::basic_string_view<CharT> str)
    {
        return green_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_yellow(const CharT* str)
    {
        return yellow_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_yellow(const CharT* str, std::size_t len)
    {
        return yellow_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_yellow(const std::basic_string<CharT>& str)
    {
        return yellow_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_yellow(const std::basic_string_view<CharT> str)
    {
        return yellow_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_blue(const CharT* str)
    {
        return blue_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_blue(const CharT* str, std::size_t len)
    {
        return blue_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_blue(const std::basic_string<CharT>& str)
    {
        return blue_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_blue(const std::basic_string_view<CharT> str)
    {
        return blue_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_magenta(const CharT* str)
    {
        return magenta_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_magenta(const CharT* str, std::size_t len)
    {
        return magenta_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_magenta(const std::basic_string<CharT>& str)
    {
        return magenta_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_magenta(const std::basic_string_view<CharT> str)
    {
        return magenta_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_cyan(const CharT* str)
    {
        return cyan_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_cyan(const CharT* str, std::size_t len)
    {
        return cyan_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_cyan(const std::basic_string<CharT>& str)
    {
        return cyan_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_cyan(const std::basic_string_view<CharT> str)
    {
        return cyan_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_white(const CharT* str)
    {
        return white_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_white(const CharT* str, std::size_t len)
    {
        return white_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_white(const std::basic_string<CharT>& str)
    {
        return white_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_white(const std::basic_string_view<CharT> str)
    {
        return white_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }


    //
    // Background manipulators
    //
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_gray(const CharT* str)
    {
        return on_gray_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_gray(const CharT* str, std::size_t len)
    {
        return on_gray_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_gray(const std::basic_string<CharT>& str)
    {
        return on_gray_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_gray(const std::basic_string_view<CharT> str)
    {
        return on_gray_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_red(const CharT* str)
    {
        return on_red_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_red(const CharT* str, std::size_t len)
    {
        return on_red_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_red(const std::basic_string<CharT>& str)
    {
        return on_red_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_red(const std::basic_string_view<CharT> str)
    {
        return on_red_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_green(const CharT* str)
    {
        return on_green_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_green(const CharT* str, std::size_t len)
    {
        return on_green_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_green(const std::basic_string<CharT>& str)
    {
        return on_green_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_green(const std::basic_string_view<CharT> str)
    {
        return on_green_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_yellow(const CharT* str)
    {
        return on_yellow_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_yellow(const CharT* str, std::size_t len)
    {
        return on_yellow_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_yellow(const std::basic_string<CharT>& str)
    {
        return on_yellow_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_yellow(const std::basic_string_view<CharT> str)
    {
        return on_yellow_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_blue(const CharT* str)
    {
        return on_blue_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_blue(const CharT* str, std::size_t len)
    {
        return on_blue_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_blue(const std::basic_string<CharT>& str)
    {
        return on_blue_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_blue(const std::basic_string_view<CharT> str)
    {
        return on_blue_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_magenta(const CharT* str)
    {
        return on_magenta_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_magenta(const CharT* str, std::size_t len)
    {
        return on_magenta_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_magenta(const std::basic_string<CharT>& str)
    {
        return on_magenta_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_magenta(const std::basic_string_view<CharT> str)
    {
        return on_magenta_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_cyan(const CharT* str)
    {
        return on_cyan_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_cyan(const CharT* str, std::size_t len)
    {
        return on_cyan_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_cyan(const std::basic_string<CharT>& str)
    {
        return on_cyan_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_cyan(const std::basic_string_view<CharT> str)
    {
        return on_cyan_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_white(const CharT* str)
    {
        return on_white_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_white(const CharT* str, std::size_t len)
    {
        return on_white_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_white(const std::basic_string<CharT>& str)
    {
        return on_white_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_on_white(const std::basic_string_view<CharT> str)
    {
        return on_white_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }


    //
    // Attribute manipulators
    //
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_bold(const CharT* str)
    {
        return bold_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_bold(const CharT* str, std::size_t len)
    {
        return bold_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_bold(const std::basic_string<CharT>& str)
    {
        return bold_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_bold(const std::basic_string_view<CharT> str)
    {
        return bold_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_dark(const CharT* str)
    {
        return dark_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_dark(const CharT* str, std::size_t len)
    {
        return dark_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_dark(const std::basic_string<CharT>& str)
    {
        return dark_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_dark(const std::basic_string_view<CharT> str)
    {
        return dark_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_underline(const CharT* str)
    {
        return underline_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_underline(const CharT* str, std::size_t len)
    {
        return underline_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_underline(const std::basic_string<CharT>& str)
    {
        return underline_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_underline(const std::basic_string_view<CharT> str)
    {
        return underline_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_blink(const CharT* str)
    {
        return blink_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_blink(const CharT* str, std::size_t len)
    {
        return blink_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_blink(const std::basic_string<CharT>& str)
    {
        return blink_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_blink(const std::basic_string_view<CharT> str)
    {
        return blink_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_reverse(const CharT* str)
    {
        return reverse_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_reverse(const CharT* str, std::size_t len)
    {
        return reverse_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_reverse(const std::basic_string<CharT>& str)
    {
        return reverse_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_reverse(const std::basic_string_view<CharT> str)
    {
        return reverse_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }

    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_concealed(const CharT* str)
    {
        return concealed_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_concealed(const CharT* str, std::size_t len)
    {
        return concealed_v<CharT>() + std::basic_string<CharT>(str, len) + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_concealed(const std::basic_string<CharT>& str)
    {
        return concealed_v<CharT>() + str + reset_v<CharT>();
    }
    template <typename CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    to_concealed(const std::basic_string_view<CharT> str)
    {
        return concealed_v<CharT>() + std::basic_string<CharT>(str) + reset_v<CharT>();
    }
}

#endif	// !TERMCOLOR2_TO_COLOR_HPP
