#ifndef TERMCOLOR2_PRESETS_HPP
#define TERMCOLOR2_PRESETS_HPP

#include <string>
#include <type_traits>
#include <poac/util/termcolor2/config.hpp>

namespace termcolor2 {
    template <typename T>
    concept Character = requires(T&&) {
        std::disjunction_v<
            std::is_same<std::remove_cvref_t<T>, char>,
            std::is_same<std::remove_cvref_t<T>, wchar_t>,
            std::is_same<std::remove_cvref_t<T>, char8_t>,
            std::is_same<std::remove_cvref_t<T>, char16_t>,
            std::is_same<std::remove_cvref_t<T>, char32_t>,
            std::is_convertible<char, std::remove_cvref_t<T>>
        >;
    };

    //
    // Foreground manipulators
    //
    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    gray_v() { return "\x1b[30m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    gray_v<wchar_t>() { return L"\x1b[30m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    gray_v<char8_t>() { return u8"\x1b[30m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    gray_v<char16_t>() { return u"\x1b[30m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    gray_v<char32_t>() { return U"\x1b[30m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto gray = gray_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    red_v() { return "\x1b[31m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    red_v<wchar_t>() { return L"\x1b[31m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    red_v<char8_t>() { return u8"\x1b[31m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    red_v<char16_t>() { return u"\x1b[31m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    red_v<char32_t>() { return U"\x1b[31m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto red = red_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    green_v() { return "\x1b[32m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    green_v<wchar_t>() { return L"\x1b[32m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    green_v<char8_t>() { return u8"\x1b[32m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    green_v<char16_t>() { return u"\x1b[32m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    green_v<char32_t>() { return U"\x1b[32m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto green = green_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    yellow_v() { return "\x1b[33m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    yellow_v<wchar_t>() { return L"\x1b[33m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    yellow_v<char8_t>() { return u8"\x1b[33m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    yellow_v<char16_t>() { return u"\x1b[33m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    yellow_v<char32_t>() { return U"\x1b[33m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto yellow = yellow_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    blue_v() { return "\x1b[34m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    blue_v<wchar_t>() { return L"\x1b[34m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    blue_v<char8_t>() { return u8"\x1b[34m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    blue_v<char16_t>() { return u"\x1b[34m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    blue_v<char32_t>() { return U"\x1b[34m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto blue = blue_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    magenta_v() { return "\x1b[35m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    magenta_v<wchar_t>() { return L"\x1b[35m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    magenta_v<char8_t>() { return u8"\x1b[35m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    magenta_v<char16_t>() { return u"\x1b[35m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    magenta_v<char32_t>() { return U"\x1b[35m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto magenta = magenta_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    cyan_v() { return "\x1b[36m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    cyan_v<wchar_t>() { return L"\x1b[36m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    cyan_v<char8_t>() { return u8"\x1b[36m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    cyan_v<char16_t>() { return u"\x1b[36m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    cyan_v<char32_t>() { return U"\x1b[36m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto cyan = cyan_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    white_v() { return "\x1b[37m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    white_v<wchar_t>() { return L"\x1b[37m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    white_v<char8_t>() { return u8"\x1b[37m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    white_v<char16_t>() { return u"\x1b[37m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    white_v<char32_t>() { return U"\x1b[37m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto white = white_v();


    //
    // Background manipulators
    //
    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    on_gray_v() { return "\x1b[40m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    on_gray_v<wchar_t>() { return L"\x1b[40m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    on_gray_v<char8_t>() { return u8"\x1b[40m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    on_gray_v<char16_t>() { return u"\x1b[40m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    on_gray_v<char32_t>() { return U"\x1b[40m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto on_gray = on_gray_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    on_red_v() { return "\x1b[41m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    on_red_v<wchar_t>() { return L"\x1b[41m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    on_red_v<char8_t>() { return u8"\x1b[41m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    on_red_v<char16_t>() { return u"\x1b[41m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    on_red_v<char32_t>() { return U"\x1b[41m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto on_red = on_red_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    on_green_v() { return "\x1b[42m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    on_green_v<wchar_t>() { return L"\x1b[42m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    on_green_v<char8_t>() { return u8"\x1b[42m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    on_green_v<char16_t>() { return u"\x1b[42m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    on_green_v<char32_t>() { return U"\x1b[42m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto on_green = on_green_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    on_yellow_v() { return "\x1b[43m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    on_yellow_v<wchar_t>() { return L"\x1b[43m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    on_yellow_v<char8_t>() { return u8"\x1b[43m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    on_yellow_v<char16_t>() { return u"\x1b[43m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    on_yellow_v<char32_t>() { return U"\x1b[43m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto on_yellow = on_yellow_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    on_blue_v() { return "\x1b[44m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    on_blue_v<wchar_t>() { return L"\x1b[44m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    on_blue_v<char8_t>() { return u8"\x1b[44m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    on_blue_v<char16_t>() { return u"\x1b[44m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    on_blue_v<char32_t>() { return U"\x1b[44m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto on_blue = on_blue_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    on_magenta_v() { return "\x1b[45m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    on_magenta_v<wchar_t>() { return L"\x1b[45m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    on_magenta_v<char8_t>() { return u8"\x1b[45m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    on_magenta_v<char16_t>() { return u"\x1b[45m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    on_magenta_v<char32_t>() { return U"\x1b[45m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto on_magenta = on_magenta_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    on_cyan_v() { return "\x1b[46m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    on_cyan_v<wchar_t>() { return L"\x1b[46m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    on_cyan_v<char8_t>() { return u8"\x1b[46m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    on_cyan_v<char16_t>() { return u"\x1b[46m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    on_cyan_v<char32_t>() { return U"\x1b[46m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto on_cyan = on_cyan_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    on_white_v() { return "\x1b[47m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    on_white_v<wchar_t>() { return L"\x1b[47m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    on_white_v<char8_t>() { return u8"\x1b[47m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    on_white_v<char16_t>() { return u"\x1b[47m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    on_white_v<char32_t>() { return U"\x1b[47m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto on_white = on_white_v();


    //
    // Attribute manipulators
    //
    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    bold_v() { return "\x1b[1m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    bold_v<wchar_t>() { return L"\x1b[1m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    bold_v<char8_t>() { return u8"\x1b[1m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    bold_v<char16_t>() { return u"\x1b[1m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    bold_v<char32_t>() { return U"\x1b[1m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto bold = bold_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    dark_v() { return "\x1b[2m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    dark_v<wchar_t>() { return L"\x1b[2m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    dark_v<char8_t>() { return u8"\x1b[2m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    dark_v<char16_t>() { return u"\x1b[2m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    dark_v<char32_t>() { return U"\x1b[2m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto dark = dark_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    underline_v() { return "\x1b[4m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    underline_v<wchar_t>() { return L"\x1b[4m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    underline_v<char8_t>() { return u8"\x1b[4m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    underline_v<char16_t>() { return u"\x1b[4m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    underline_v<char32_t>() { return U"\x1b[4m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto underline = underline_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    blink_v() { return "\x1b[5m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    blink_v<wchar_t>() { return L"\x1b[5m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    blink_v<char8_t>() { return u8"\x1b[5m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    blink_v<char16_t>() { return u"\x1b[5m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    blink_v<char32_t>() { return U"\x1b[5m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto blink = blink_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    reverse_v() { return "\x1b[7m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    reverse_v<wchar_t>() { return L"\x1b[7m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    reverse_v<char8_t>() { return u8"\x1b[7m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    reverse_v<char16_t>() { return u"\x1b[7m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    reverse_v<char32_t>() { return U"\x1b[7m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto reverse = reverse_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    concealed_v() { return "\x1b[8m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    concealed_v<wchar_t>() { return L"\x1b[8m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    concealed_v<char8_t>() { return u8"\x1b[8m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    concealed_v<char16_t>() { return u"\x1b[8m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    concealed_v<char32_t>() { return U"\x1b[8m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto concealed = concealed_v();


    template <typename CharT = char>
    requires Character<CharT>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<CharT>
    reset_v() { return "\x1b[0m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<wchar_t>
    reset_v<wchar_t>() { return L"\x1b[0m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char8_t>
    reset_v<char8_t>() { return u8"\x1b[0m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char16_t>
    reset_v<char16_t>() { return u"\x1b[0m"; }

    template <>
    inline TERMCOLOR2_CXX20_CONSTEVAL std::basic_string<char32_t>
    reset_v<char32_t>() { return U"\x1b[0m"; }

    inline TERMCOLOR2_CXX20_CONSTINIT const auto reset = reset_v();
} // end namespace termcolor2

#endif // !TERMCOLOR2_PRESETS_HPP
