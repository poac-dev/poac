#ifndef TERMCOLOR2_MIN_HPP
#define TERMCOLOR2_MIN_HPP

namespace termcolor2 {
    template <typename T>
    constexpr const T& min(const T& a, const T& b) {
        return b < a ? b : a;
    }
} // end namespace termcolor2

#endif	// !TERMCOLOR2_MIN_HPP
