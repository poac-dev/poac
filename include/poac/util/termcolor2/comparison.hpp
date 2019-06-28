#ifndef TERMCOLOR2_COMPARISON_HPP
#define TERMCOLOR2_COMPARISON_HPP

#include "./string.hpp"

namespace termcolor2 {
    template <typename CharT, std::size_t N1, std::size_t N2, typename Traits>
    constexpr bool
    operator==(const basic_string<CharT, N1, Traits>& lhs, const basic_string<CharT, N2, Traits>& rhs) {
        return lhs.compare(rhs) == 0;
    }
    template <typename CharT, std::size_t N, typename Traits>
    constexpr bool
    operator==(const basic_string<CharT, N, Traits>& lhs, const CharT* rhs) {
        return lhs.compare(rhs) == 0;
    }
    template <typename CharT, std::size_t N, typename Traits>
    constexpr bool
    operator==(const CharT* lhs, const basic_string<CharT, N, Traits>& rhs) {
        return 0 == rhs.compare(lhs);
    }
    template <typename CharT, std::size_t N1, std::size_t N2, typename Traits>
    constexpr bool
    operator!=(const basic_string<CharT, N1, Traits>& lhs, const basic_string<CharT, N2, Traits>& rhs) {
        return !(lhs == rhs);
    }
    template <typename CharT, std::size_t N, typename Traits>
    constexpr bool
    operator!=(const basic_string<CharT, N, Traits>& lhs, const CharT* rhs) {
        return !(lhs == rhs);
    }
    template <typename CharT, std::size_t N, typename Traits>
    constexpr bool
    operator!=(const CharT* lhs, const basic_string<CharT, N, Traits>& rhs) {
        return !(lhs == rhs);
    }
} // end namespace termcolor2

#endif	// !TERMCOLOR2_COMPARISON_HPP
