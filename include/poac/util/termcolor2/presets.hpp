#ifndef TERMCOLOR2_PRESETS_HPP
#define TERMCOLOR2_PRESETS_HPP

#include "./string.hpp"

namespace termcolor2 {
    template <typename CharT = char>
    constexpr basic_string red = make_string("\x1b[31m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string red<wchar_t> = make_string(L"\x1b[31m");
    template <>
    constexpr basic_string red<char16_t> = make_string(u"\x1b[31m");
    template <>
    constexpr basic_string red<char32_t> = make_string(U"\x1b[31m");
#endif

    template <typename CharT = char>
    constexpr basic_string green = make_string("\x1b[32m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string green<wchar_t> = make_string(L"\x1b[32m");
    template <>
    constexpr basic_string green<char16_t> = make_string(u"\x1b[32m");
    template <>
    constexpr basic_string green<char32_t> = make_string(U"\x1b[32m");
#endif

    template <typename CharT = char>
    constexpr basic_string yellow = make_string("\x1b[33m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string yellow<wchar_t> = make_string(L"\x1b[33m");
    template <>
    constexpr basic_string yellow<char16_t> = make_string(u"\x1b[33m");
    template <>
    constexpr basic_string yellow<char32_t> = make_string(U"\x1b[33m");
#endif

    template <typename CharT = char>
    constexpr basic_string blue = make_string("\x1b[34m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string blue<wchar_t> = make_string(L"\x1b[34m");
    template <>
    constexpr basic_string blue<char16_t> = make_string(u"\x1b[34m");
    template <>
    constexpr basic_string blue<char32_t> = make_string(U"\x1b[34m");
#endif

    template <typename CharT = char>
    constexpr basic_string pink = make_string("\x1b[35m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string pink<wchar_t> = make_string(L"\x1b[35m");
    template <>
    constexpr basic_string pink<char16_t> = make_string(u"\x1b[35m");
    template <>
    constexpr basic_string pink<char32_t> = make_string(U"\x1b[35m");
#endif

    template <typename CharT = char>
    constexpr basic_string gray = make_string("\x1b[90m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string gray<wchar_t> = make_string(L"\x1b[90m");
    template <>
    constexpr basic_string gray<char16_t> = make_string(u"\x1b[90m");
    template <>
    constexpr basic_string gray<char32_t> = make_string(U"\x1b[90m");
#endif

    template <typename CharT = char>
    constexpr basic_string bold = make_string("\x1b[1m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string bold<wchar_t> = make_string(L"\x1b[1m");
    template <>
    constexpr basic_string bold<char16_t> = make_string(u"\x1b[1m");
    template <>
    constexpr basic_string bold<char32_t> = make_string(U"\x1b[1m");
#endif

    template <typename CharT = char>
    constexpr basic_string underline = make_string("\x1b[4m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string underline<wchar_t> = make_string(L"\x1b[4m");
    template <>
    constexpr basic_string underline<char16_t> = make_string(u"\x1b[4m");
    template <>
    constexpr basic_string underline<char32_t> = make_string(U"\x1b[4m");
#endif

    template <typename CharT = char>
    constexpr basic_string reset = make_string("\x1b[0m");
#ifndef _MSC_VER
    template <>
    constexpr basic_string reset<wchar_t> = make_string(L"\x1b[0m");
    template <>
    constexpr basic_string reset<char16_t> = make_string(u"\x1b[0m");
    template <>
    constexpr basic_string reset<char32_t> = make_string(U"\x1b[0m");
#endif
} // end namespace termcolor2

#endif	// !TERMCOLOR2_PRESETS_HPP
