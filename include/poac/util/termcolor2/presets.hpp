#ifndef TERMCOLOR2_PRESETS_HPP
#define TERMCOLOR2_PRESETS_HPP

#include <poac/util/termcolor2/string.hpp>

namespace termcolor2 {
    //
    // Foreground manipulators
    //
    template <typename CharT = char>
    inline constexpr auto gray_v = make_string("\x1b[30m");
    template <>
    inline constexpr auto gray_v<wchar_t> = make_string(L"\x1b[30m");
    template <>
    inline constexpr auto gray_v<char16_t> = make_string(u"\x1b[30m");
    template <>
    inline constexpr auto gray_v<char32_t> = make_string(U"\x1b[30m");
    inline constexpr auto gray = gray_v<char>;

    template <typename CharT = char>
    inline constexpr auto red_v = make_string("\x1b[31m");
    template <>
    inline constexpr auto red_v<wchar_t> = make_string(L"\x1b[31m");
    template <>
    inline constexpr auto red_v<char16_t> = make_string(u"\x1b[31m");
    template <>
    inline constexpr auto red_v<char32_t> = make_string(U"\x1b[31m");
    inline constexpr auto red = red_v<char>;

    template <typename CharT = char>
    inline constexpr auto green_v = make_string("\x1b[32m");
    template <>
    inline constexpr auto green_v<wchar_t> = make_string(L"\x1b[32m");
    template <>
    inline constexpr auto green_v<char16_t> = make_string(u"\x1b[32m");
    template <>
    inline constexpr auto green_v<char32_t> = make_string(U"\x1b[32m");
    inline constexpr auto green = green_v<char>;

    template <typename CharT = char>
    inline constexpr auto yellow_v = make_string("\x1b[33m");
    template <>
    inline constexpr auto yellow_v<wchar_t> = make_string(L"\x1b[33m");
    template <>
    inline constexpr auto yellow_v<char16_t> = make_string(u"\x1b[33m");
    template <>
    inline constexpr auto yellow_v<char32_t> = make_string(U"\x1b[33m");
    inline constexpr auto yellow = yellow_v<char>;

    template <typename CharT = char>
    inline constexpr auto blue_v = make_string("\x1b[34m");
    template <>
    inline constexpr auto blue_v<wchar_t> = make_string(L"\x1b[34m");
    template <>
    inline constexpr auto blue_v<char16_t> = make_string(u"\x1b[34m");
    template <>
    inline constexpr auto blue_v<char32_t> = make_string(U"\x1b[34m");
    inline constexpr auto blue = blue_v<char>;

    template <typename CharT = char>
    inline constexpr auto magenta_v = make_string("\x1b[35m");
    template <>
    inline constexpr auto magenta_v<wchar_t> = make_string(L"\x1b[35m");
    template <>
    inline constexpr auto magenta_v<char16_t> = make_string(u"\x1b[35m");
    template <>
    inline constexpr auto magenta_v<char32_t> = make_string(U"\x1b[35m");
    inline constexpr auto magenta = magenta_v<char>;

    template <typename CharT = char>
    inline constexpr auto cyan_v = make_string("\x1b[36m");
    template <>
    inline constexpr auto cyan_v<wchar_t> = make_string(L"\x1b[36m");
    template <>
    inline constexpr auto cyan_v<char16_t> = make_string(u"\x1b[36m");
    template <>
    inline constexpr auto cyan_v<char32_t> = make_string(U"\x1b[36m");
    inline constexpr auto cyan = cyan_v<char>;

    template <typename CharT = char>
    inline constexpr auto white_v = make_string("\x1b[37m");
    template <>
    inline constexpr auto white_v<wchar_t> = make_string(L"\x1b[37m");
    template <>
    inline constexpr auto white_v<char16_t> = make_string(u"\x1b[37m");
    template <>
    inline constexpr auto white_v<char32_t> = make_string(U"\x1b[37m");
    inline constexpr auto white = white_v<char>;

    //
    // Background manipulators
    //
    template <typename CharT = char>
    inline constexpr auto on_gray_v = make_string("\x1b[40m");
    template <>
    inline constexpr auto on_gray_v<wchar_t> = make_string(L"\x1b[40m");
    template <>
    inline constexpr auto on_gray_v<char16_t> = make_string(u"\x1b[40m");
    template <>
    inline constexpr auto on_gray_v<char32_t> = make_string(U"\x1b[40m");
    inline constexpr auto on_gray = on_gray_v<char>;

    template <typename CharT = char>
    inline constexpr auto on_red_v = make_string("\x1b[41m");
    template <>
    inline constexpr auto on_red_v<wchar_t> = make_string(L"\x1b[41m");
    template <>
    inline constexpr auto on_red_v<char16_t> = make_string(u"\x1b[41m");
    template <>
    inline constexpr auto on_red_v<char32_t> = make_string(U"\x1b[41m");
    inline constexpr auto on_red = on_red_v<char>;

    template <typename CharT = char>
    inline constexpr auto on_green_v = make_string("\x1b[42m");
    template <>
    inline constexpr auto on_green_v<wchar_t> = make_string(L"\x1b[42m");
    template <>
    inline constexpr auto on_green_v<char16_t> = make_string(u"\x1b[42m");
    template <>
    inline constexpr auto on_green_v<char32_t> = make_string(U"\x1b[42m");
    inline constexpr auto on_green = on_green_v<char>;

    template <typename CharT = char>
    inline constexpr auto on_yellow_v = make_string("\x1b[43m");
    template <>
    inline constexpr auto on_yellow_v<wchar_t> = make_string(L"\x1b[43m");
    template <>
    inline constexpr auto on_yellow_v<char16_t> = make_string(u"\x1b[43m");
    template <>
    inline constexpr auto on_yellow_v<char32_t> = make_string(U"\x1b[43m");
    inline constexpr auto on_yellow = on_yellow_v<char>;

    template <typename CharT = char>
    inline constexpr auto on_blue_v = make_string("\x1b[44m");
    template <>
    inline constexpr auto on_blue_v<wchar_t> = make_string(L"\x1b[44m");
    template <>
    inline constexpr auto on_blue_v<char16_t> = make_string(u"\x1b[44m");
    template <>
    inline constexpr auto on_blue_v<char32_t> = make_string(U"\x1b[44m");
    inline constexpr auto on_blue = on_blue_v<char>;

    template <typename CharT = char>
    inline constexpr auto on_magenta_v = make_string("\x1b[45m");
    template <>
    inline constexpr auto on_magenta_v<wchar_t> = make_string(L"\x1b[45m");
    template <>
    inline constexpr auto on_magenta_v<char16_t> = make_string(u"\x1b[45m");
    template <>
    inline constexpr auto on_magenta_v<char32_t> = make_string(U"\x1b[45m");
    inline constexpr auto on_magenta = on_magenta_v<char>;

    template <typename CharT = char>
    inline constexpr auto on_cyan_v = make_string("\x1b[46m");
    template <>
    inline constexpr auto on_cyan_v<wchar_t> = make_string(L"\x1b[46m");
    template <>
    inline constexpr auto on_cyan_v<char16_t> = make_string(u"\x1b[46m");
    template <>
    inline constexpr auto on_cyan_v<char32_t> = make_string(U"\x1b[46m");
    inline constexpr auto on_cyan = on_cyan_v<char>;

    template <typename CharT = char>
    inline constexpr auto on_white_v = make_string("\x1b[47m");
    template <>
    inline constexpr auto on_white_v<wchar_t> = make_string(L"\x1b[47m");
    template <>
    inline constexpr auto on_white_v<char16_t> = make_string(u"\x1b[47m");
    template <>
    inline constexpr auto on_white_v<char32_t> = make_string(U"\x1b[47m");
    inline constexpr auto on_white = on_white_v<char>;

    //
    // Attribute manipulators
    //
    template <typename CharT = char>
    inline constexpr auto bold_v = make_string("\x1b[1m");
    template <>
    inline constexpr auto bold_v<wchar_t> = make_string(L"\x1b[1m");
    template <>
    inline constexpr auto bold_v<char16_t> = make_string(u"\x1b[1m");
    template <>
    inline constexpr auto bold_v<char32_t> = make_string(U"\x1b[1m");
    inline constexpr auto bold = bold_v<char>;

    template <typename CharT = char>
    inline constexpr auto dark_v = make_string("\x1b[2m");
    template <>
    inline constexpr auto dark_v<wchar_t> = make_string(L"\x1b[2m");
    template <>
    inline constexpr auto dark_v<char16_t> = make_string(u"\x1b[2m");
    template <>
    inline constexpr auto dark_v<char32_t> = make_string(U"\x1b[2m");
    inline constexpr auto dark = dark_v<char>;

    template <typename CharT = char>
    inline constexpr auto underline_v = make_string("\x1b[4m");
    template <>
    inline constexpr auto underline_v<wchar_t> = make_string(L"\x1b[4m");
    template <>
    inline constexpr auto underline_v<char16_t> = make_string(u"\x1b[4m");
    template <>
    inline constexpr auto underline_v<char32_t> = make_string(U"\x1b[4m");
    inline constexpr auto underline = underline_v<char>;

    template <typename CharT = char>
    inline constexpr auto blink_v = make_string("\x1b[5m");
    template <>
    inline constexpr auto blink_v<wchar_t> = make_string(L"\x1b[5m");
    template <>
    inline constexpr auto blink_v<char16_t> = make_string(u"\x1b[5m");
    template <>
    inline constexpr auto blink_v<char32_t> = make_string(U"\x1b[5m");
    inline constexpr auto blink = blink_v<char>;

    template <typename CharT = char>
    inline constexpr auto reverse_v = make_string("\x1b[7m");
    template <>
    inline constexpr auto reverse_v<wchar_t> = make_string(L"\x1b[7m");
    template <>
    inline constexpr auto reverse_v<char16_t> = make_string(u"\x1b[7m");
    template <>
    inline constexpr auto reverse_v<char32_t> = make_string(U"\x1b[7m");
    inline constexpr auto reverse = reverse_v<char>;

    template <typename CharT = char>
    inline constexpr auto concealed_v = make_string("\x1b[8m");
    template <>
    inline constexpr auto concealed_v<wchar_t> = make_string(L"\x1b[8m");
    template <>
    inline constexpr auto concealed_v<char16_t> = make_string(u"\x1b[8m");
    template <>
    inline constexpr auto concealed_v<char32_t> = make_string(U"\x1b[8m");
    inline constexpr auto concealed = concealed_v<char>;

    template <typename CharT = char>
    inline constexpr auto reset_v = make_string("\x1b[0m");
    template <>
    inline constexpr auto reset_v<wchar_t> = make_string(L"\x1b[0m");
    template <>
    inline constexpr auto reset_v<char16_t> = make_string(u"\x1b[0m");
    template <>
    inline constexpr auto reset_v<char32_t> = make_string(U"\x1b[0m");
    inline constexpr auto reset = reset_v<char>;
} // end namespace termcolor2

#endif	// !TERMCOLOR2_PRESETS_HPP
