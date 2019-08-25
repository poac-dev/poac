#ifndef TERMCOLOR2_ALIAS_HPP
#define TERMCOLOR2_ALIAS_HPP

#include <poac/util/termcolor2/string.hpp>

namespace termcolor2 {
    template <std::size_t N>
    using string = basic_string<char, N>;

    template <std::size_t N>
    using wstring = basic_string<wchar_t, N>;

    template <std::size_t N>
    using u16string = basic_string<char16_t, N>;

    template <std::size_t N>
    using u32string = basic_string<char32_t, N>;
} // end namespace termcolor2

#endif	// !TERMCOLOR2_ALIAS_HPP
