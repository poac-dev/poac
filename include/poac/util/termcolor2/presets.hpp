#ifndef TERMCOLOR2_PRESETS_HPP
#define TERMCOLOR2_PRESETS_HPP

#include "./string.hpp"

namespace termcolor2 {
    template <typename CharT = char>
    constexpr auto red = make_string("\x1b[31m");
#ifndef _MSC_VER
    template <>
    constexpr auto red<wchar_t> = make_string(L"\x1b[31m");
    template <>
    constexpr auto red<char16_t> = make_string(u"\x1b[31m");
    template <>
    constexpr auto red<char32_t> = make_string(U"\x1b[31m");
#endif

    template <typename CharT = char>
    constexpr auto green = make_string("\x1b[32m");
#ifndef _MSC_VER
    template <>
    constexpr auto green<wchar_t> = make_string(L"\x1b[32m");
    template <>
    constexpr auto green<char16_t> = make_string(u"\x1b[32m");
    template <>
    constexpr auto green<char32_t> = make_string(U"\x1b[32m");
#endif

    template <typename CharT = char>
    constexpr auto yellow = make_string("\x1b[33m");
#ifndef _MSC_VER
    template <>
    constexpr auto yellow<wchar_t> = make_string(L"\x1b[33m");
    template <>
    constexpr auto yellow<char16_t> = make_string(u"\x1b[33m");
    template <>
    constexpr auto yellow<char32_t> = make_string(U"\x1b[33m");
#endif

    template <typename CharT = char>
    constexpr auto blue = make_string("\x1b[34m");
#ifndef _MSC_VER
    template <>
    constexpr auto blue<wchar_t> = make_string(L"\x1b[34m");
    template <>
    constexpr auto blue<char16_t> = make_string(u"\x1b[34m");
    template <>
    constexpr auto blue<char32_t> = make_string(U"\x1b[34m");
#endif

    template <typename CharT = char>
    constexpr auto pink = make_string("\x1b[35m");
#ifndef _MSC_VER
    template <>
    constexpr auto pink<wchar_t> = make_string(L"\x1b[35m");
    template <>
    constexpr auto pink<char16_t> = make_string(u"\x1b[35m");
    template <>
    constexpr auto pink<char32_t> = make_string(U"\x1b[35m");
#endif

    template <typename CharT = char>
    constexpr auto gray = make_string("\x1b[90m");
#ifndef _MSC_VER
    template <>
    constexpr auto gray<wchar_t> = make_string(L"\x1b[90m");
    template <>
    constexpr auto gray<char16_t> = make_string(u"\x1b[90m");
    template <>
    constexpr auto gray<char32_t> = make_string(U"\x1b[90m");
#endif

    template <typename CharT = char>
    constexpr auto bold = make_string("\x1b[1m");
#ifndef _MSC_VER
    template <>
    constexpr auto bold<wchar_t> = make_string(L"\x1b[1m");
    template <>
    constexpr auto bold<char16_t> = make_string(u"\x1b[1m");
    template <>
    constexpr auto bold<char32_t> = make_string(U"\x1b[1m");
#endif

    template <typename CharT = char>
    constexpr auto underline = make_string("\x1b[4m");
#ifndef _MSC_VER
    template <>
    constexpr auto underline<wchar_t> = make_string(L"\x1b[4m");
    template <>
    constexpr auto underline<char16_t> = make_string(u"\x1b[4m");
    template <>
    constexpr auto underline<char32_t> = make_string(U"\x1b[4m");
#endif

    template <typename CharT = char>
    constexpr auto reset = make_string("\x1b[0m");
#ifndef _MSC_VER
    template <>
    constexpr auto reset<wchar_t> = make_string(L"\x1b[0m");
    template <>
    constexpr auto reset<char16_t> = make_string(u"\x1b[0m");
    template <>
    constexpr auto reset<char32_t> = make_string(U"\x1b[0m");
#endif
} // end namespace termcolor2

#endif	// !TERMCOLOR2_PRESETS_HPP
