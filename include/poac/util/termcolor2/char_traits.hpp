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

        static TERMCOLOR2_CXX14_CONSTEXPR void
        assign(char_type& c1, const char_type& c2) noexcept {
            c1 = c2;
        }
        static TERMCOLOR2_CXX14_CONSTEXPR char_type*
        assign(char_type* s, std::size_t n, char_type a) {
            char_type* _r = s;
            for (; n; --n, ++s) {
                assign(*s, a);
            }
            return _r;
        }

        static constexpr bool
        eq(char_type c1, char_type c2) noexcept {
			return c1 == c2;
		}
		static constexpr bool
		lt(char_type c1, char_type c2) noexcept {
			return c1 < c2;
		}

        static constexpr int
        compare(const char_type* s1, const char_type* s2, std::size_t n) {
            return n == 0
                   ? 0
                   : lt(*s1, *s2)
                     ? -1
                     : lt(*s2, *s1)
                       ? 1
                       : compare(s1 + 1, s2 + 1, n - 1)
                    ;
        }

    public:
        static constexpr std::size_t
        length(const char_type* s) {
            return length_impl(s, 0);
        }

    private:
        static constexpr std::size_t
        length_impl(const char_type* s, std::size_t len) {
            return !eq(*s, char_type(0))
                   ? length_impl(s + 1, len + 1)
                   : len;
        }

    public:
        static constexpr const char_type*
        find(const char_type* s, std::size_t n, const char_type& a) {
            return n == 0
                   ? 0
                   : eq(*s, a)
                     ? s
                     : find(s + 1, n - 1, a)
                    ;
        }

        static TERMCOLOR2_CXX14_CONSTEXPR char_type*
        move(char_type* s1, const char_type* s2, std::size_t n) {
            char_type* _r = s1;
            if (s1 < s2) {
                for (; n; --n, ++s1, ++s2) {
                    assign(*s1, *s2);
                }
            } else if (s2 < s1) {
                s1 += n;
                s2 += n;
                for (; n; --n) {
                    assign(*--s1, *--s2);
                }
            }
            return _r;
        }

        static TERMCOLOR2_CXX14_CONSTEXPR char_type*
        copy(char_type* s1, const char_type* s2, std::size_t n) {
            static_assert(s2 < s1 || s2 >= s1 + n, "termcolor2::char_traits::copy overlapped range");
            char_type* _r = s1;
            for (; n; --n, ++s1, ++s2) {
                assign(*s1, *s2);
            }
            return _r;
        }

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
