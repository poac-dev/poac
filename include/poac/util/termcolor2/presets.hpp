#ifndef TERMCOLOR2_PRESETS_HPP
#define TERMCOLOR2_PRESETS_HPP

#include <poac/util/termcolor2/string.hpp>

namespace termcolor2 {
    //
    // Foreground manipulators
    //
    template <typename CharT = char>
    constexpr auto gray = make_string("\x1b[30m");
#ifndef _MSC_VER
    template <>
    constexpr auto gray<wchar_t> = make_string(L"\x1b[30m");
    template <>
    constexpr auto gray<char16_t> = make_string(u"\x1b[30m");
    template <>
    constexpr auto gray<char32_t> = make_string(U"\x1b[30m");
#endif

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
    constexpr auto magenta = make_string("\x1b[35m");
#ifndef _MSC_VER
    template <>
    constexpr auto magenta<wchar_t> = make_string(L"\x1b[35m");
    template <>
    constexpr auto magenta<char16_t> = make_string(u"\x1b[35m");
    template <>
    constexpr auto magenta<char32_t> = make_string(U"\x1b[35m");
#endif

    template <typename CharT = char>
    constexpr auto cyan = make_string("\x1b[36m");
#ifndef _MSC_VER
    template <>
    constexpr auto cyan<wchar_t> = make_string(L"\x1b[36m");
    template <>
    constexpr auto cyan<char16_t> = make_string(u"\x1b[36m");
    template <>
    constexpr auto cyan<char32_t> = make_string(U"\x1b[36m");
#endif

    template <typename CharT = char>
    constexpr auto white = make_string("\x1b[37m");
#ifndef _MSC_VER
    template <>
    constexpr auto white<wchar_t> = make_string(L"\x1b[37m");
    template <>
    constexpr auto white<char16_t> = make_string(u"\x1b[37m");
    template <>
    constexpr auto white<char32_t> = make_string(U"\x1b[37m");
#endif

    //
    // Background manipulators
    //
    template <typename CharT = char>
    constexpr auto on_gray = make_string("\x1b[40m");
#ifndef _MSC_VER
    template <>
    constexpr auto on_gray<wchar_t> = make_string(L"\x1b[40m");
    template <>
    constexpr auto on_gray<char16_t> = make_string(u"\x1b[40m");
    template <>
    constexpr auto on_gray<char32_t> = make_string(U"\x1b[40m");
#endif

    template <typename CharT = char>
    constexpr auto on_red = make_string("\x1b[41m");
#ifndef _MSC_VER
    template <>
    constexpr auto on_red<wchar_t> = make_string(L"\x1b[41m");
    template <>
    constexpr auto on_red<char16_t> = make_string(u"\x1b[41m");
    template <>
    constexpr auto on_red<char32_t> = make_string(U"\x1b[41m");
#endif

    template <typename CharT = char>
    constexpr auto on_green = make_string("\x1b[42m");
#ifndef _MSC_VER
    template <>
    constexpr auto on_green<wchar_t> = make_string(L"\x1b[42m");
    template <>
    constexpr auto on_green<char16_t> = make_string(u"\x1b[42m");
    template <>
    constexpr auto on_green<char32_t> = make_string(U"\x1b[42m");
#endif

    template <typename CharT = char>
    constexpr auto on_yellow = make_string("\x1b[43m");
#ifndef _MSC_VER
    template <>
    constexpr auto on_yellow<wchar_t> = make_string(L"\x1b[43m");
    template <>
    constexpr auto on_yellow<char16_t> = make_string(u"\x1b[43m");
    template <>
    constexpr auto on_yellow<char32_t> = make_string(U"\x1b[43m");
#endif

    template <typename CharT = char>
    constexpr auto on_blue = make_string("\x1b[44m");
#ifndef _MSC_VER
    template <>
    constexpr auto on_blue<wchar_t> = make_string(L"\x1b[44m");
    template <>
    constexpr auto on_blue<char16_t> = make_string(u"\x1b[44m");
    template <>
    constexpr auto on_blue<char32_t> = make_string(U"\x1b[44m");
#endif

    template <typename CharT = char>
    constexpr auto on_magenta = make_string("\x1b[45m");
#ifndef _MSC_VER
    template <>
    constexpr auto on_magenta<wchar_t> = make_string(L"\x1b[45m");
    template <>
    constexpr auto on_magenta<char16_t> = make_string(u"\x1b[45m");
    template <>
    constexpr auto on_magenta<char32_t> = make_string(U"\x1b[45m");
#endif

    template <typename CharT = char>
    constexpr auto on_cyan = make_string("\x1b[46m");
#ifndef _MSC_VER
    template <>
    constexpr auto on_cyan<wchar_t> = make_string(L"\x1b[46m");
    template <>
    constexpr auto on_cyan<char16_t> = make_string(u"\x1b[46m");
    template <>
    constexpr auto on_cyan<char32_t> = make_string(U"\x1b[46m");
#endif

    template <typename CharT = char>
    constexpr auto on_white = make_string("\x1b[47m");
#ifndef _MSC_VER
    template <>
    constexpr auto on_white<wchar_t> = make_string(L"\x1b[47m");
    template <>
    constexpr auto on_white<char16_t> = make_string(u"\x1b[47m");
    template <>
    constexpr auto on_white<char32_t> = make_string(U"\x1b[47m");
#endif

    //
    // Attribute manipulators
    //
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
    constexpr auto dark = make_string("\x1b[2m");
#ifndef _MSC_VER
    template <>
    constexpr auto dark<wchar_t> = make_string(L"\x1b[2m");
    template <>
    constexpr auto dark<char16_t> = make_string(u"\x1b[2m");
    template <>
    constexpr auto dark<char32_t> = make_string(U"\x1b[2m");
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
    constexpr auto blink = make_string("\x1b[5m");
#ifndef _MSC_VER
    template <>
    constexpr auto blink<wchar_t> = make_string(L"\x1b[5m");
    template <>
    constexpr auto blink<char16_t> = make_string(u"\x1b[5m");
    template <>
    constexpr auto blink<char32_t> = make_string(U"\x1b[5m");
#endif

    template <typename CharT = char>
    constexpr auto reverse = make_string("\x1b[7m");
#ifndef _MSC_VER
    template <>
    constexpr auto reverse<wchar_t> = make_string(L"\x1b[7m");
    template <>
    constexpr auto reverse<char16_t> = make_string(u"\x1b[7m");
    template <>
    constexpr auto reverse<char32_t> = make_string(U"\x1b[7m");
#endif

    template <typename CharT = char>
    constexpr auto concealed = make_string("\x1b[8m");
#ifndef _MSC_VER
    template <>
    constexpr auto concealed<wchar_t> = make_string(L"\x1b[8m");
    template <>
    constexpr auto concealed<char16_t> = make_string(u"\x1b[8m");
    template <>
    constexpr auto concealed<char32_t> = make_string(U"\x1b[8m");
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
