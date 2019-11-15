#ifndef TERMCOLOR2_LITERALS_HPP
#define TERMCOLOR2_LITERALS_HPP

#include <cstddef> // std::size_t
#include <string> // std::basic_string
#include <poac/util/termcolor2/to_color.hpp>

namespace termcolor2 {
inline namespace color_literals {
inline namespace foreground_literals {
#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, gray_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _gray() noexcept
    {
        return to_gray<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _gray(const char* str, std::size_t len) noexcept
    {
        return to_gray(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _gray(const wchar_t* str, std::size_t len) noexcept
    {
        return to_gray(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _gray(const char16_t* str, std::size_t len) noexcept
    {
        return to_gray(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _gray(const char32_t* str, std::size_t len) noexcept
    {
        return to_gray(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, red_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _red() noexcept
    {
        return to_red<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _red(const char* str, std::size_t len) noexcept
    {
        return to_red(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _red(const wchar_t* str, std::size_t len) noexcept
    {
        return to_red(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _red(const char16_t* str, std::size_t len) noexcept
    {
        return to_red(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _red(const char32_t* str, std::size_t len) noexcept
    {
        return to_red(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, green_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _green() noexcept
    {
        return to_green<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _green(const char* str, std::size_t len) noexcept
    {
        return to_green(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _green(const wchar_t* str, std::size_t len) noexcept
    {
        return to_green(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _green(const char16_t* str, std::size_t len) noexcept
    {
        return to_green(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _green(const char32_t* str, std::size_t len) noexcept
    {
        return to_green(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, yellow_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _yellow() noexcept
    {
        return to_yellow<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _yellow(const char* str, std::size_t len) noexcept
    {
        return to_yellow(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _yellow(const wchar_t* str, std::size_t len) noexcept
    {
        return to_yellow(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _yellow(const char16_t* str, std::size_t len) noexcept
    {
        return to_yellow(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _yellow(const char32_t* str, std::size_t len) noexcept
    {
        return to_yellow(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, blue_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _blue() noexcept
    {
        return to_blue<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _blue(const char* str, std::size_t len) noexcept
    {
        return to_blue(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _blue(const wchar_t* str, std::size_t len) noexcept
    {
        return to_blue(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _blue(const char16_t* str, std::size_t len) noexcept
    {
        return to_blue(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _blue(const char32_t* str, std::size_t len) noexcept
    {
        return to_blue(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, magenta_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _magenta() noexcept
    {
        return to_magenta<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _magenta(const char* str, std::size_t len) noexcept
    {
        return to_magenta(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _magenta(const wchar_t* str, std::size_t len) noexcept
    {
        return to_magenta(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _magenta(const char16_t* str, std::size_t len) noexcept
    {
        return to_magenta(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _magenta(const char32_t* str, std::size_t len) noexcept
    {
        return to_magenta(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, cyan_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _cyan() noexcept
    {
        return to_cyan<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _cyan(const char* str, std::size_t len) noexcept
    {
        return to_cyan(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _cyan(const wchar_t* str, std::size_t len) noexcept
    {
        return to_cyan(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _cyan(const char16_t* str, std::size_t len) noexcept
    {
        return to_cyan(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _cyan(const char32_t* str, std::size_t len) noexcept
    {
        return to_cyan(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, white_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _white() noexcept
    {
        return to_white<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _white(const char* str, std::size_t len) noexcept
    {
        return to_white(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _white(const wchar_t* str, std::size_t len) noexcept
    {
        return to_white(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _white(const char16_t* str, std::size_t len) noexcept
    {
        return to_white(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _white(const char32_t* str, std::size_t len) noexcept
    {
        return to_white(str, len);
    }
#endif
} // end namespace foreground


inline namespace background_literals {
#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, on_gray_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _on_gray() noexcept
    {
        return to_on_gray<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _on_gray(const char* str, std::size_t len) noexcept
    {
        return to_on_gray(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _on_gray(const wchar_t* str, std::size_t len) noexcept
    {
        return to_on_gray(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _on_gray(const char16_t* str, std::size_t len) noexcept
    {
        return to_on_gray(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _on_gray(const char32_t* str, std::size_t len) noexcept
    {
        return to_on_gray(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, on_red_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _on_red() noexcept
    {
        return to_on_red<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _on_red(const char* str, std::size_t len) noexcept
    {
        return to_on_red(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _on_red(const wchar_t* str, std::size_t len) noexcept
    {
        return to_on_red(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _on_red(const char16_t* str, std::size_t len) noexcept
    {
        return to_on_red(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _on_red(const char32_t* str, std::size_t len) noexcept
    {
        return to_on_red(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, on_green_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _on_green() noexcept
    {
        return to_on_green<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _on_green(const char* str, std::size_t len) noexcept
    {
        return to_on_green(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _on_green(const wchar_t* str, std::size_t len) noexcept
    {
        return to_on_green(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _on_green(const char16_t* str, std::size_t len) noexcept
    {
        return to_on_green(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _on_green(const char32_t* str, std::size_t len) noexcept
    {
        return to_on_green(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, on_yellow_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _on_yellow() noexcept
    {
        return to_on_yellow<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _on_yellow(const char* str, std::size_t len) noexcept
    {
        return to_on_yellow(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _on_yellow(const wchar_t* str, std::size_t len) noexcept
    {
        return to_on_yellow(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _on_yellow(const char16_t* str, std::size_t len) noexcept
    {
        return to_on_yellow(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _on_yellow(const char32_t* str, std::size_t len) noexcept
    {
        return to_on_yellow(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, on_blue_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _on_blue() noexcept
    {
        return to_on_blue<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _on_blue(const char* str, std::size_t len) noexcept
    {
        return to_on_blue(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _on_blue(const wchar_t* str, std::size_t len) noexcept
    {
        return to_on_blue(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _on_blue(const char16_t* str, std::size_t len) noexcept
    {
        return to_on_blue(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _on_blue(const char32_t* str, std::size_t len) noexcept
    {
        return to_on_blue(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, on_magenta_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _on_magenta() noexcept
    {
        return to_on_magenta<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _on_magenta(const char* str, std::size_t len) noexcept
    {
        return to_on_magenta(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _on_magenta(const wchar_t* str, std::size_t len) noexcept
    {
        return to_on_magenta(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _on_magenta(const char16_t* str, std::size_t len) noexcept
    {
        return to_on_magenta(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _on_magenta(const char32_t* str, std::size_t len) noexcept
    {
        return to_on_magenta(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, on_cyan_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _on_cyan() noexcept
    {
        return to_on_cyan<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _on_cyan(const char* str, std::size_t len) noexcept
    {
        return to_on_cyan(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _on_cyan(const wchar_t* str, std::size_t len) noexcept
    {
        return to_on_cyan(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _on_cyan(const char16_t* str, std::size_t len) noexcept
    {
        return to_on_cyan(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _on_cyan(const char32_t* str, std::size_t len) noexcept
    {
        return to_on_cyan(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, on_white_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _on_white() noexcept
    {
        return to_on_white<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _on_white(const char* str, std::size_t len) noexcept
    {
        return to_on_white(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _on_white(const wchar_t* str, std::size_t len) noexcept
    {
        return to_on_white(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _on_white(const char16_t* str, std::size_t len) noexcept
    {
        return to_on_white(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _on_white(const char32_t* str, std::size_t len) noexcept
    {
        return to_on_white(str, len);
    }
#endif
} // end namespace background_literals


inline namespace attribute_literals {
#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, bold_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _bold() noexcept
    {
        return to_bold<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _bold(const char* str, std::size_t len) noexcept
    {
        return to_bold(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _bold(const wchar_t* str, std::size_t len) noexcept
    {
        return to_bold(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _bold(const char16_t* str, std::size_t len) noexcept
    {
        return to_bold(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _bold(const char32_t* str, std::size_t len) noexcept
    {
        return to_bold(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, dark_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _dark() noexcept
    {
        return to_dark<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _dark(const char* str, std::size_t len) noexcept
    {
        return to_dark(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _dark(const wchar_t* str, std::size_t len) noexcept
    {
        return to_dark(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _dark(const char16_t* str, std::size_t len) noexcept
    {
        return to_dark(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _dark(const char32_t* str, std::size_t len) noexcept
    {
        return to_dark(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, underline_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _underline() noexcept
    {
        return to_underline<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _underline(const char* str, std::size_t len) noexcept
    {
        return to_underline(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _underline(const wchar_t* str, std::size_t len) noexcept
    {
        return to_underline(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _underline(const char16_t* str, std::size_t len) noexcept
    {
        return to_underline(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _underline(const char32_t* str, std::size_t len) noexcept
    {
        return to_underline(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, blink_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _blink() noexcept
    {
        return to_blink<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _blink(const char* str, std::size_t len) noexcept
    {
        return to_blink(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _blink(const wchar_t* str, std::size_t len) noexcept
    {
        return to_blink(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _blink(const char16_t* str, std::size_t len) noexcept
    {
        return to_blink(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _blink(const char32_t* str, std::size_t len) noexcept
    {
        return to_blink(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, reverse_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _reverse() noexcept
    {
        return to_reverse<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _reverse(const char* str, std::size_t len) noexcept
    {
        return to_reverse(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _reverse(const wchar_t* str, std::size_t len) noexcept
    {
        return to_reverse(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _reverse(const char16_t* str, std::size_t len) noexcept
    {
        return to_reverse(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _reverse(const char32_t* str, std::size_t len) noexcept
    {
        return to_reverse(str, len);
    }
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
    template <typename CharT, CharT... Str>
    constexpr basic_string<CharT, concealed_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
    operator "" _concealed() noexcept
    {
        return to_concealed<CharT, sizeof...(Str), Str...>();
    }
#else
    inline std::basic_string<char>
    operator "" _concealed(const char* str, std::size_t len) noexcept
    {
        return to_concealed(str, len);
    }
    inline std::basic_string<wchar_t>
    operator "" _concealed(const wchar_t* str, std::size_t len) noexcept
    {
        return to_concealed(str, len);
    }
    inline std::basic_string<char16_t>
    operator "" _concealed(const char16_t* str, std::size_t len) noexcept
    {
        return to_concealed(str, len);
    }
    inline std::basic_string<char32_t>
    operator "" _concealed(const char32_t* str, std::size_t len) noexcept
    {
        return to_concealed(str, len);
    }
#endif
} // end namespace attribute_literals


inline namespace control_literals {
} // end namespace control_literals
} // end namespace color_literals
} // end namespace termcolor2

#endif	// !TERMCOLOR2_LITERALS_HPP
