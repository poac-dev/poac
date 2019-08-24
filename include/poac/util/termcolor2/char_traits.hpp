#ifndef TERMCOLOR2_CHAR_TRAITS_HPP
#define TERMCOLOR2_CHAR_TRAITS_HPP

#include <string> // std::char_traits
#include <poac/util/termcolor2/config.hpp>

namespace termcolor2 {
    template <typename CharT>
    struct char_traits {
        using std_traits_type = std::char_traits<CharT>;
        using char_type = typename std_traits_type::char_type;
        using int_type = typename std_traits_type::int_type;
        using off_type = typename std_traits_type::off_type;
        using pos_type = typename std_traits_type::pos_type;
        using state_type = typename std_traits_type::state_type;

#if TERMCOLOR2_CHAR_TRAITS_AFTER_CXX17
        static constexpr void
        assign(char_type& c1, const char_type& c2) noexcept {
            std_traits_type::assign(c1, c2);
        }
        static constexpr char_type*
        assign(char_type* s, std::size_t n, char_type a) {
            return std_traits_type::assign(c1, c2);
        }
#else
        static constexpr char_type*
        assign(char_type* s, std::size_t n, char_type a) { // TODO: Support C++11
            char_type* _r = s;
            for (; n; --n, ++s) {
                assign(*s, a);
            }
            return _r;
        }
#  if TERMCOLOR2_CHAR_TRAITS_AFTER_CXX14
        static constexpr void
        assign(char_type& c1, const char_type& c2) noexcept {
            std_traits_type::assign(c1, c2);
        }
#  else
        static constexpr void
        assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        }
#  endif
#endif

#if TERMCOLOR2_CHAR_TRAITS_AFTER_CXX11
        static constexpr bool
        eq(char_type c1, char_type c2) noexcept {
            return std_traits_type::eq(c1, c2);
        }
        static constexpr bool
        lt(char_type c1, char_type c2) noexcept {
            return std_traits_type::lt(c1, c2);
        }
#else
        static constexpr bool
        eq(char_type c1, char_type c2) noexcept {
			return c1 == c2;
		}
		static constexpr bool
		lt(char_type c1, char_type c2) noexcept {
			return c1 < c2;
		}
#endif

#if TERMCOLOR2_CHAR_TRAITS_AFTER_CXX14
        static constexpr int
        compare(const char_type* s1, const char_type* s2, std::size_t n) {
            return std_traits_type::compare(s1, s2, n);
        }
        static constexpr std::size_t
        length(const char_type* s) {
#if defined(__clang__) && !defined(__APPLE__) // TODO: Support C++11
            // Clang(does not contain Apple Clang):
            //   non-constexpr function 'wcslen' cannot be used in a constant expression
            if constexpr (std::is_same<value_type, wchar_t>::value) {
                size_type _len = 0;
                for (; !traits_type::eq(*str, value_type(0)); ++str, ++_len);
                return _len;
            }
#else
            return std_traits_type::length(s);
#endif
        }
        static constexpr const char_type*
        find(const char_type* s, std::size_t n, const char_type& a) {
            return std_traits_type::find(s, n, a);
        }
#else
        static constexpr int
        compare(const char_type* s1, const char_type* s2, std::size_t n) { // TODO: Support C++11
            for (; n; --n, ++s1, ++s2) {
                if (lt(*s1, *s2)) {
                    return -1;
                }
                if (lt(*s2, *s1)) {
                    return 1;
                }
            }
            return 0;
        }
        static constexpr std::size_t
        length(const char_type* s) { // TODO: Support C++11
            size_t _len = 0;
            for (; !eq(*s, char_type(0)); ++s, ++_len);
            return _len;
        }
        static constexpr const char_type*
        find(const char_type* s, std::size_t n, const char_type& a) { // TODO: Support C++11
            for (; n; --n) {
                if (eq(*s, a)) {
                    return s;
                }
                ++s;
            }
            return 0;
        }
#endif

#if TERMCOLOR2_CHAR_TRAITS_AFTER_CXX17
        static constexpr char_type*
        move(char_type* s1, const char_type* s2, std::size_t n) {
            return std_traits_type::move(s1, s2, n);
        }
        static constexpr char_type*
        copy(char_type* s1, const char_type* s2, std::size_t n) {
            return std_traits_type::copy(s1, s2, n);
        }
#else
        static constexpr char_type*
        move(char_type* s1, const char_type* s2, std::size_t n) { // TODO: Support C++11
            char_type* _r = s1;
            if (s1 < s2) {
                for (; n; --n, ++s1, ++s2) {
                    assign(*s1, *s2);
                }
            }
            else if (s2 < s1) {
                s1 += n;
                s2 += n;
                for (; n; --n) {
                    assign(*--s1, *--s2);
                }
            }
            return _r;
        }
        static constexpr char_type*
        copy(char_type* s1, const char_type* s2, std::size_t n) { // TODO: Support C++11
            static_assert(s2 < s1 || s2 >= s1 + n, "termcolor2::char_traits::copy overlapped range");
            char_type* _r = s1;
            for (; n; --n, ++s1, ++s2) {
                assign(*s1, *s2);
            }
            return _r;
        }
#endif

        static constexpr int_type
        not_eof(int_type c) noexcept {
            return std_traits_type::not_eof(c);
        }

        static constexpr char_type
        to_char_type(int_type c) noexcept {
            return std_traits_type::to_char_type(c);
        }

        static constexpr int_type
        to_int_type(char_type c) noexcept {
            return std_traits_type::to_int_type(c);
        }

        static constexpr bool
        eq_int_type(int_type c1, int_type c2) noexcept {
            return std_traits_type::eq_int_type(c1, c2);
        }

        static constexpr int_type
        eof() noexcept {
            return std_traits_type::eof();
        }
    };
} // end namespace termcolor2

#endif	// !TERMCOLOR2_CHAR_TRAITS_HPP
