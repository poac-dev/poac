#ifndef TERMCOLOR2_PRESETS_HPP
#define TERMCOLOR2_PRESETS_HPP

#include <type_traits>
#include <poac/util/termcolor2/string.hpp>

namespace termcolor2 {
    //
    // Foreground manipulators
    //
    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    gray_v() {
        return make_string("\x1b[30m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    gray_v() {
        return make_string(L"\x1b[30m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    gray_v() {
        return make_string(u"\x1b[30m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    gray_v() {
        return make_string(U"\x1b[30m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto gray = gray_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    red_v() {
        return make_string("\x1b[31m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    red_v() {
        return make_string(L"\x1b[31m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    red_v() {
        return make_string(u"\x1b[31m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    red_v() {
        return make_string(U"\x1b[31m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto red = red_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    green_v() {
        return make_string("\x1b[32m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    green_v() {
        return make_string(L"\x1b[32m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    green_v() {
        return make_string(u"\x1b[32m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    green_v() {
        return make_string(U"\x1b[32m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto green = green_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    yellow_v() {
        return make_string("\x1b[33m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    yellow_v() {
        return make_string(L"\x1b[33m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    yellow_v() {
        return make_string(u"\x1b[33m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    yellow_v() {
        return make_string(U"\x1b[33m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto yellow = yellow_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    blue_v() {
        return make_string("\x1b[34m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    blue_v() {
        return make_string(L"\x1b[34m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    blue_v() {
        return make_string(u"\x1b[34m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    blue_v() {
        return make_string(U"\x1b[34m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto blue = blue_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    magenta_v() {
        return make_string("\x1b[35m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    magenta_v() {
        return make_string(L"\x1b[35m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    magenta_v() {
        return make_string(u"\x1b[35m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    magenta_v() {
        return make_string(U"\x1b[35m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto magenta = magenta_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    cyan_v() {
        return make_string("\x1b[36m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    cyan_v() {
        return make_string(L"\x1b[36m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    cyan_v() {
        return make_string(u"\x1b[36m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    cyan_v() {
        return make_string(U"\x1b[36m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto cyan = cyan_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    white_v() {
        return make_string("\x1b[37m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    white_v() {
        return make_string(L"\x1b[37m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    white_v() {
        return make_string(u"\x1b[37m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    white_v() {
        return make_string(U"\x1b[37m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto white = white_v();


    //
    // Background manipulators
    //
    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_gray_v() {
        return make_string("\x1b[40m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_gray_v() {
        return make_string(L"\x1b[40m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_gray_v() {
        return make_string(u"\x1b[40m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_gray_v() {
        return make_string(U"\x1b[40m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto on_gray = on_gray_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_red_v() {
        return make_string("\x1b[41m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_red_v() {
        return make_string(L"\x1b[41m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_red_v() {
        return make_string(u"\x1b[41m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_red_v() {
        return make_string(U"\x1b[41m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto on_red = on_red_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_green_v() {
        return make_string("\x1b[42m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_green_v() {
        return make_string(L"\x1b[42m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_green_v() {
        return make_string(u"\x1b[42m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_green_v() {
        return make_string(U"\x1b[42m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto on_green = on_green_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_yellow_v() {
        return make_string("\x1b[43m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_yellow_v() {
        return make_string(L"\x1b[43m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_yellow_v() {
        return make_string(u"\x1b[43m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_yellow_v() {
        return make_string(U"\x1b[43m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto on_yellow = on_yellow_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_blue_v() {
        return make_string("\x1b[44m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_blue_v() {
        return make_string(L"\x1b[44m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_blue_v() {
        return make_string(u"\x1b[44m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_blue_v() {
        return make_string(U"\x1b[44m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto on_blue = on_blue_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_magenta_v() {
        return make_string("\x1b[45m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_magenta_v() {
        return make_string(L"\x1b[45m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_magenta_v() {
        return make_string(u"\x1b[45m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_magenta_v() {
        return make_string(U"\x1b[45m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto on_magenta = on_magenta_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_cyan_v() {
        return make_string("\x1b[46m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_cyan_v() {
        return make_string(L"\x1b[46m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_cyan_v() {
        return make_string(u"\x1b[46m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_cyan_v() {
        return make_string(U"\x1b[46m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto on_cyan = on_cyan_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_white_v() {
        return make_string("\x1b[47m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_white_v() {
        return make_string(L"\x1b[47m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_white_v() {
        return make_string(u"\x1b[47m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 5>
    on_white_v() {
        return make_string(U"\x1b[47m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto on_white = on_white_v();


    //
    // Attribute manipulators
    //
    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    bold_v() {
        return make_string("\x1b[1m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    bold_v() {
        return make_string(L"\x1b[1m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    bold_v() {
        return make_string(u"\x1b[1m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    bold_v() {
        return make_string(U"\x1b[1m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto bold = bold_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    dark_v() {
        return make_string("\x1b[2m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    dark_v() {
        return make_string(L"\x1b[2m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    dark_v() {
        return make_string(u"\x1b[2m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    dark_v() {
        return make_string(U"\x1b[2m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto dark = dark_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    underline_v() {
        return make_string("\x1b[4m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    underline_v() {
        return make_string(L"\x1b[4m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    underline_v() {
        return make_string(u"\x1b[4m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    underline_v() {
        return make_string(U"\x1b[4m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto underline = underline_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    blink_v() {
        return make_string("\x1b[5m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    blink_v() {
        return make_string(L"\x1b[5m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    blink_v() {
        return make_string(u"\x1b[5m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    blink_v() {
        return make_string(U"\x1b[5m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto blink = blink_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    reverse_v() {
        return make_string("\x1b[7m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    reverse_v() {
        return make_string(L"\x1b[7m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    reverse_v() {
        return make_string(u"\x1b[7m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    reverse_v() {
        return make_string(U"\x1b[7m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto reverse = reverse_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    concealed_v() {
        return make_string("\x1b[8m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    concealed_v() {
        return make_string(L"\x1b[8m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    concealed_v() {
        return make_string(u"\x1b[8m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    concealed_v() {
        return make_string(U"\x1b[8m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto concealed = concealed_v();


    template <typename CharT = char,
        typename std::enable_if<
            std::is_same<CharT, char>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    reset_v() {
        return make_string("\x1b[0m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, wchar_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    reset_v() {
        return make_string(L"\x1b[0m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char16_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    reset_v() {
        return make_string(u"\x1b[0m");
    }
    template <typename CharT,
        typename std::enable_if<
            std::is_same<CharT, char32_t>::value
            , std::nullptr_t
        >::type = nullptr
    >
    constexpr basic_string<CharT, 4>
    reset_v() {
        return make_string(U"\x1b[0m");
    }
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14
    constexpr auto reset = reset_v();
} // end namespace termcolor2

#endif // !TERMCOLOR2_PRESETS_HPP
