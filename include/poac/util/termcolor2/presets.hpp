#ifndef TERMCOLOR2_PRESETS_HPP
#define TERMCOLOR2_PRESETS_HPP

#include <poac/util/termcolor2/string.hpp>

namespace termcolor2 {
    //
    // Foreground manipulators
    //
    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray_v = make_string("\x1b[30m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray_v<wchar_t> = make_string(L"\x1b[30m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray_v<char16_t> = make_string(u"\x1b[30m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray_v<char32_t> = make_string(U"\x1b[30m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray = gray_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red_v = make_string("\x1b[31m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red_v<wchar_t> = make_string(L"\x1b[31m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red_v<char16_t> = make_string(u"\x1b[31m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red_v<char32_t> = make_string(U"\x1b[31m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red = red_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green_v = make_string("\x1b[32m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green_v<wchar_t> = make_string(L"\x1b[32m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green_v<char16_t> = make_string(u"\x1b[32m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green_v<char32_t> = make_string(U"\x1b[32m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green = green_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow_v = make_string("\x1b[33m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow_v<wchar_t> = make_string(L"\x1b[33m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow_v<char16_t> = make_string(u"\x1b[33m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow_v<char32_t> = make_string(U"\x1b[33m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow = yellow_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue_v = make_string("\x1b[34m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue_v<wchar_t> = make_string(L"\x1b[34m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue_v<char16_t> = make_string(u"\x1b[34m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue_v<char32_t> = make_string(U"\x1b[34m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue = blue_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta_v = make_string("\x1b[35m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta_v<wchar_t> = make_string(L"\x1b[35m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta_v<char16_t> = make_string(u"\x1b[35m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta_v<char32_t> = make_string(U"\x1b[35m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta = magenta_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan_v = make_string("\x1b[36m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan_v<wchar_t> = make_string(L"\x1b[36m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan_v<char16_t> = make_string(u"\x1b[36m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan_v<char32_t> = make_string(U"\x1b[36m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan = cyan_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white_v = make_string("\x1b[37m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white_v<wchar_t> = make_string(L"\x1b[37m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white_v<char16_t> = make_string(u"\x1b[37m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white_v<char32_t> = make_string(U"\x1b[37m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white = white_v<char>;

    //
    // Background manipulators
    //
    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray_v = make_string("\x1b[40m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray_v<wchar_t> = make_string(L"\x1b[40m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray_v<char16_t> = make_string(u"\x1b[40m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray_v<char32_t> = make_string(U"\x1b[40m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray = on_gray_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red_v = make_string("\x1b[41m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red_v<wchar_t> = make_string(L"\x1b[41m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red_v<char16_t> = make_string(u"\x1b[41m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red_v<char32_t> = make_string(U"\x1b[41m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red = on_red_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green_v = make_string("\x1b[42m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green_v<wchar_t> = make_string(L"\x1b[42m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green_v<char16_t> = make_string(u"\x1b[42m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green_v<char32_t> = make_string(U"\x1b[42m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green = on_green_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow_v = make_string("\x1b[43m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow_v<wchar_t> = make_string(L"\x1b[43m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow_v<char16_t> = make_string(u"\x1b[43m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow_v<char32_t> = make_string(U"\x1b[43m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow = on_yellow_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue_v = make_string("\x1b[44m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue_v<wchar_t> = make_string(L"\x1b[44m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue_v<char16_t> = make_string(u"\x1b[44m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue_v<char32_t> = make_string(U"\x1b[44m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue = on_blue_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta_v = make_string("\x1b[45m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta_v<wchar_t> = make_string(L"\x1b[45m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta_v<char16_t> = make_string(u"\x1b[45m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta_v<char32_t> = make_string(U"\x1b[45m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta = on_magenta_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan_v = make_string("\x1b[46m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan_v<wchar_t> = make_string(L"\x1b[46m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan_v<char16_t> = make_string(u"\x1b[46m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan_v<char32_t> = make_string(U"\x1b[46m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan = on_cyan_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white_v = make_string("\x1b[47m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white_v<wchar_t> = make_string(L"\x1b[47m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white_v<char16_t> = make_string(u"\x1b[47m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white_v<char32_t> = make_string(U"\x1b[47m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white = on_white_v<char>;

    //
    // Attribute manipulators
    //
    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold_v = make_string("\x1b[1m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold_v<wchar_t> = make_string(L"\x1b[1m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold_v<char16_t> = make_string(u"\x1b[1m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold_v<char32_t> = make_string(U"\x1b[1m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold = bold_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark_v = make_string("\x1b[2m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark_v<wchar_t> = make_string(L"\x1b[2m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark_v<char16_t> = make_string(u"\x1b[2m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark_v<char32_t> = make_string(U"\x1b[2m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark = dark_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline_v = make_string("\x1b[4m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline_v<wchar_t> = make_string(L"\x1b[4m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline_v<char16_t> = make_string(u"\x1b[4m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline_v<char32_t> = make_string(U"\x1b[4m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline = underline_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink_v = make_string("\x1b[5m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink_v<wchar_t> = make_string(L"\x1b[5m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink_v<char16_t> = make_string(u"\x1b[5m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink_v<char32_t> = make_string(U"\x1b[5m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink = blink_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse_v = make_string("\x1b[7m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse_v<wchar_t> = make_string(L"\x1b[7m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse_v<char16_t> = make_string(u"\x1b[7m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse_v<char32_t> = make_string(U"\x1b[7m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse = reverse_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed_v = make_string("\x1b[8m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed_v<wchar_t> = make_string(L"\x1b[8m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed_v<char16_t> = make_string(u"\x1b[8m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed_v<char32_t> = make_string(U"\x1b[8m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed = concealed_v<char>;

    template <typename CharT = char>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset_v = make_string("\x1b[0m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset_v<wchar_t> = make_string(L"\x1b[0m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset_v<char16_t> = make_string(u"\x1b[0m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset_v<char32_t> = make_string(U"\x1b[0m");
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset = reset_v<char>;
} // end namespace termcolor2

#endif // !TERMCOLOR2_PRESETS_HPP
