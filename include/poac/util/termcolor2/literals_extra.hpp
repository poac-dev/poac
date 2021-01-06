#ifndef TERMCOLOR2_LITERALS_EXTRA_HPP
#define TERMCOLOR2_LITERALS_EXTRA_HPP

#include <cstddef> // std::size_t
#include <string> // std::basic_string
#include <poac/util/termcolor2/to_color.hpp>

namespace termcolor2 {
    inline namespace color_literals {
        inline namespace foreground_literals {
            inline std::basic_string<char>
            operator "" _bold_green(const char* str, std::size_t len) noexcept
            {
                return to_bold(to_green(str, len));
            }
            inline std::basic_string<wchar_t>
            operator "" _bold_green(const wchar_t* str, std::size_t len) noexcept
            {
                return to_bold(to_green(str, len));
            }
            inline std::basic_string<char16_t>
            operator "" _bold_green(const char16_t* str, std::size_t len) noexcept
            {
                return to_bold(to_green(str, len));
            }
            inline std::basic_string<char32_t>
            operator "" _bold_green(const char32_t* str, std::size_t len) noexcept
            {
                return to_bold(to_green(str, len));
            }
        } // end namespace foreground_literals
    } // end namespace color_literals
} // end namespace termcolor2

#endif	// !TERMCOLOR2_LITERALS_EXTRA_HPP
