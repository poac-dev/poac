#ifndef TERMCOLOR2_LITERALS_EXTRA_HPP
#define TERMCOLOR2_LITERALS_EXTRA_HPP

#include <cstddef> // std::size_t
#include <string> // std::basic_string
#include <poac/util/termcolor2/presets.hpp>

namespace termcolor2::inline color_literals::inline foreground_literals {
    inline std::basic_string<char>
    operator "" _bold_red(const char* str, std::size_t len)
    {
        return bold_v<char>()
             + red_v<char>()
             + std::basic_string<char>(str, len)
             + reset_v<char>();
    }
    inline std::basic_string<wchar_t>
    operator "" _bold_red(const wchar_t* str, std::size_t len)
    {
        return bold_v<wchar_t>()
             + red_v<wchar_t>()
             + std::basic_string<wchar_t>(str, len)
             + reset_v<wchar_t>();
    }
    inline std::basic_string<char8_t>
    operator "" _bold_red(const char8_t* str, std::size_t len)
    {
        return bold_v<char8_t>()
             + red_v<char8_t>()
             + std::basic_string<char8_t>(str, len)
             + reset_v<char8_t>();
    }
    inline std::basic_string<char16_t>
    operator "" _bold_red(const char16_t* str, std::size_t len)
    {
        return bold_v<char16_t>()
             + red_v<char16_t>()
             + std::basic_string<char16_t>(str, len)
             + reset_v<char16_t>();
    }
    inline std::basic_string<char32_t>
    operator "" _bold_red(const char32_t* str, std::size_t len)
    {
        return bold_v<char32_t>()
             + red_v<char32_t>()
             + std::basic_string<char32_t>(str, len)
             + reset_v<char32_t>();
    }

    inline std::basic_string<char>
    operator "" _bold_green(const char* str, std::size_t len)
    {
        return bold_v<char>()
             + green_v<char>()
             + std::basic_string<char>(str, len)
             + reset_v<char>();
    }
    inline std::basic_string<wchar_t>
    operator "" _bold_green(const wchar_t* str, std::size_t len)
    {
        return bold_v<wchar_t>()
             + green_v<wchar_t>()
             + std::basic_string<wchar_t>(str, len)
             + reset_v<wchar_t>();
    }
    inline std::basic_string<char8_t>
    operator "" _bold_green(const char8_t* str, std::size_t len)
    {
        return bold_v<char8_t>()
             + green_v<char8_t>()
             + std::basic_string<char8_t>(str, len)
             + reset_v<char8_t>();
    }
    inline std::basic_string<char16_t>
    operator "" _bold_green(const char16_t* str, std::size_t len)
    {
        return bold_v<char16_t>()
             + green_v<char16_t>()
             + std::basic_string<char16_t>(str, len)
             + reset_v<char16_t>();
    }
    inline std::basic_string<char32_t>
    operator "" _bold_green(const char32_t* str, std::size_t len)
    {
        return bold_v<char32_t>()
             + green_v<char32_t>()
             + std::basic_string<char32_t>(str, len)
             + reset_v<char32_t>();
    }

    inline std::basic_string<char>
    operator "" _bold_yellow(const char* str, std::size_t len)
    {
        return bold_v<char>()
             + yellow_v<char>()
             + std::basic_string<char>(str, len)
             + reset_v<char>();
    }
    inline std::basic_string<wchar_t>
    operator "" _bold_yellow(const wchar_t* str, std::size_t len)
    {
        return bold_v<wchar_t>()
             + yellow_v<wchar_t>()
             + std::basic_string<wchar_t>(str, len)
             + reset_v<wchar_t>();
    }
    inline std::basic_string<char8_t>
    operator "" _bold_yellow(const char8_t* str, std::size_t len)
    {
        return bold_v<char8_t>()
             + yellow_v<char8_t>()
             + std::basic_string<char8_t>(str, len)
             + reset_v<char8_t>();
    }
    inline std::basic_string<char16_t>
    operator "" _bold_yellow(const char16_t* str, std::size_t len)
    {
        return bold_v<char16_t>()
             + yellow_v<char16_t>()
             + std::basic_string<char16_t>(str, len)
             + reset_v<char16_t>();
    }
    inline std::basic_string<char32_t>
    operator "" _bold_yellow(const char32_t* str, std::size_t len)
    {
        return bold_v<char32_t>()
             + yellow_v<char32_t>()
             + std::basic_string<char32_t>(str, len)
             + reset_v<char32_t>();
    }
} // end namespace termcolor2::color_literals::foreground_literals

#endif	// !TERMCOLOR2_LITERALS_EXTRA_HPP
