#ifndef TERMCOLOR2_PRESETS_HPP
#define TERMCOLOR2_PRESETS_HPP

#include <type_traits>
#include <poac/util/termcolor2/string.hpp>

namespace termcolor2 {
    //
    // Foreground manipulators
    //
    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray_v = make_string("\x1b[30m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray_v<wchar_t> = make_string(L"\x1b[30m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray_v<char16_t> = make_string(u"\x1b[30m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray_v<char32_t> = make_string(U"\x1b[30m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto gray = gray_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red_v = make_string("\x1b[31m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red_v<wchar_t> = make_string(L"\x1b[31m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red_v<char16_t> = make_string(u"\x1b[31m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red_v<char32_t> = make_string(U"\x1b[31m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto red = red_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green_v = make_string("\x1b[32m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green_v<wchar_t> = make_string(L"\x1b[32m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green_v<char16_t> = make_string(u"\x1b[32m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green_v<char32_t> = make_string(U"\x1b[32m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto green = green_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow_v = make_string("\x1b[33m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow_v<wchar_t> = make_string(L"\x1b[33m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow_v<char16_t> = make_string(u"\x1b[33m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow_v<char32_t> = make_string(U"\x1b[33m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto yellow = yellow_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue_v = make_string("\x1b[34m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue_v<wchar_t> = make_string(L"\x1b[34m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue_v<char16_t> = make_string(u"\x1b[34m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue_v<char32_t> = make_string(U"\x1b[34m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blue = blue_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta_v = make_string("\x1b[35m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta_v<wchar_t> = make_string(L"\x1b[35m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta_v<char16_t> = make_string(u"\x1b[35m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta_v<char32_t> = make_string(U"\x1b[35m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto magenta = magenta_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan_v = make_string("\x1b[36m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan_v<wchar_t> = make_string(L"\x1b[36m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan_v<char16_t> = make_string(u"\x1b[36m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan_v<char32_t> = make_string(U"\x1b[36m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto cyan = cyan_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white_v = make_string("\x1b[37m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white_v<wchar_t> = make_string(L"\x1b[37m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white_v<char16_t> = make_string(u"\x1b[37m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white_v<char32_t> = make_string(U"\x1b[37m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto white = white_v<char>;

    //
    // Background manipulators
    //
    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray_v = make_string("\x1b[40m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray_v<wchar_t> = make_string(L"\x1b[40m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray_v<char16_t> = make_string(u"\x1b[40m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray_v<char32_t> = make_string(U"\x1b[40m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_gray = on_gray_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red_v = make_string("\x1b[41m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red_v<wchar_t> = make_string(L"\x1b[41m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red_v<char16_t> = make_string(u"\x1b[41m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red_v<char32_t> = make_string(U"\x1b[41m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_red = on_red_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green_v = make_string("\x1b[42m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green_v<wchar_t> = make_string(L"\x1b[42m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green_v<char16_t> = make_string(u"\x1b[42m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green_v<char32_t> = make_string(U"\x1b[42m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_green = on_green_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow_v = make_string("\x1b[43m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow_v<wchar_t> = make_string(L"\x1b[43m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow_v<char16_t> = make_string(u"\x1b[43m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow_v<char32_t> = make_string(U"\x1b[43m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_yellow = on_yellow_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue_v = make_string("\x1b[44m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue_v<wchar_t> = make_string(L"\x1b[44m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue_v<char16_t> = make_string(u"\x1b[44m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue_v<char32_t> = make_string(U"\x1b[44m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_blue = on_blue_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta_v = make_string("\x1b[45m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta_v<wchar_t> = make_string(L"\x1b[45m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta_v<char16_t> = make_string(u"\x1b[45m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta_v<char32_t> = make_string(U"\x1b[45m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_magenta = on_magenta_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan_v = make_string("\x1b[46m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan_v<wchar_t> = make_string(L"\x1b[46m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan_v<char16_t> = make_string(u"\x1b[46m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan_v<char32_t> = make_string(U"\x1b[46m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_cyan = on_cyan_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white_v = make_string("\x1b[47m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white_v<wchar_t> = make_string(L"\x1b[47m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white_v<char16_t> = make_string(u"\x1b[47m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white_v<char32_t> = make_string(U"\x1b[47m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto on_white = on_white_v<char>;

    //
    // Attribute manipulators
    //
    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold_v = make_string("\x1b[1m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold_v<wchar_t> = make_string(L"\x1b[1m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold_v<char16_t> = make_string(u"\x1b[1m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold_v<char32_t> = make_string(U"\x1b[1m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto bold = bold_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark_v = make_string("\x1b[2m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark_v<wchar_t> = make_string(L"\x1b[2m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark_v<char16_t> = make_string(u"\x1b[2m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark_v<char32_t> = make_string(U"\x1b[2m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto dark = dark_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline_v = make_string("\x1b[4m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline_v<wchar_t> = make_string(L"\x1b[4m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline_v<char16_t> = make_string(u"\x1b[4m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline_v<char32_t> = make_string(U"\x1b[4m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto underline = underline_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink_v = make_string("\x1b[5m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink_v<wchar_t> = make_string(L"\x1b[5m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink_v<char16_t> = make_string(u"\x1b[5m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink_v<char32_t> = make_string(U"\x1b[5m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto blink = blink_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse_v = make_string("\x1b[7m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse_v<wchar_t> = make_string(L"\x1b[7m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse_v<char16_t> = make_string(u"\x1b[7m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse_v<char32_t> = make_string(U"\x1b[7m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reverse = reverse_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed_v = make_string("\x1b[8m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed_v<wchar_t> = make_string(L"\x1b[8m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed_v<char16_t> = make_string(u"\x1b[8m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed_v<char32_t> = make_string(U"\x1b[8m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto concealed = concealed_v<char>;

    template <typename CharT = char,
        std::enable_if_t<
            std::disjunction_v<
                std::is_same<CharT, char>,
                std::is_same<CharT, char16_t>,
                std::is_same<CharT, char32_t>,
                std::is_same<CharT, wchar_t>
            >
            , std::nullptr_t
        > = nullptr
    >
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset_v = make_string("\x1b[0m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset_v<wchar_t> = make_string(L"\x1b[0m");
#ifndef _MSC_VER
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset_v<char16_t> = make_string(u"\x1b[0m");
    template <>
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset_v<char32_t> = make_string(U"\x1b[0m");
#endif
    TERMCOLOR2_INLINE_VARIABLES_AFTER_CXX14 constexpr auto reset = reset_v<char>;
} // end namespace termcolor2

#endif // !TERMCOLOR2_PRESETS_HPP
