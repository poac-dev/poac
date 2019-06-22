#ifndef TERMCOLOR2_STRING_HPP
#define TERMCOLOR2_STRING_HPP

#include <cstddef> // std::size_t
#include <algorithm> // std::min
#include <string> // std::char_traits
#include <stdexcept> // std::length_error, std::out_of_range
#include <utility> // std::index_sequence, std::make_index_sequence, std::forward

namespace termcolor2 {
    template <typename CharT, std::size_t N, typename Traits = std::char_traits<CharT>>
    class basic_string {
    public:
        using value_type = typename Traits::char_type;
        using reference = value_type&;
        using const_reference = const value_type&;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using traits_type = Traits;
        using iterator = CharT*;
        using const_iterator = const CharT*;

    private:
        value_type elems[N + 1];
        size_type len;

    public:
        basic_string(const basic_string&) = default;
        basic_string(basic_string &&) = default;
        constexpr basic_string()
                : elems{}, len() {}

        template<typename Str, std::size_t... Indexes>
        constexpr basic_string(
                std::index_sequence<Indexes...>,
                const Str& str, size_type pos, size_type n
        ) : elems{(
                Indexes < n
                    ? static_cast<value_type>(str[Indexes + pos])
                    : value_type()
            )...},
            len(
                N >= n
                    ? n
                    : throw std::length_error("termcolor2::basic_string")
            )
        {}

        template<typename... Args, std::size_t... Indexes>
        constexpr basic_string(
                std::index_sequence<Indexes...>,
                size_type n, Args&&... args
        ) : elems{(
                Indexes < n
                    ? static_cast<value_type>(std::forward<Args>(args))
                    : value_type()
            )...},
            len(n)
        {}

        explicit constexpr basic_string(const value_type* s)
            : basic_string(
                  std::make_index_sequence<N>{},
                  s, 0, length(s)
              ) // delegation
        {}

        constexpr basic_string(std::initializer_list<value_type> il)
            : basic_string(
                  std::make_index_sequence<N>{},
                  il.begin(), 0, il.size()
              ) // delegation
        {}

        template<typename... Args, typename = typename std::enable_if_t<(sizeof...(Args) <= N)>>
        constexpr basic_string(size_type n, Args&&... args)
            : basic_string(
                  std::make_index_sequence<sizeof...(Args)>{},
                  n, std::forward<Args>(args)...
              ) // delegation
        {}

        std::basic_string<CharT, Traits>
        to_string() const noexcept {
            return c_str();
        }

        constexpr const_pointer
        c_str() const noexcept {
            return data();
        }
        constexpr pointer
        data() noexcept {
            return elems;
        }
        constexpr const_pointer
        data() const noexcept {
            return elems;
        }

        constexpr iterator
        begin() noexcept {
            return data();
        }
        constexpr const_iterator
        begin() const noexcept {
            return data();
        }

        constexpr size_type
        size() const noexcept {
            return len;
        }

        constexpr size_type
        length(const value_type* str) const noexcept {
#if defined(__clang__) && !defined(__APPLE__)
            // Clang(does not contain Apple Clang):
            //   non-constexpr function 'wcslen' cannot be used in a constant expression
            if constexpr (std::is_same_v<value_type, wchar_t>) {
                size_type _len = 0;
                for (; !traits_type::eq(*str, value_type(0)); ++str, ++_len);
                return _len;
            }
#endif
            return traits_type::length(str);
        }

        [[nodiscard]] constexpr bool
        empty() const noexcept
        {
            return size() == 0;
        }

        template <std::size_t N2>
        constexpr int
        compare(const basic_string<CharT, N2, Traits>& str) const {
            return compare(0, size(), str.begin(), str.size());
        }
        constexpr int
        compare(const value_type* s) const {
            return compare(0, size(), s, length(s));
        }
        constexpr int
        compare(size_type pos1, size_type n1, const value_type* s, size_type n2) const {
            if (size() < pos1) {
                throw std::out_of_range("termcolor2::basic_string");
            }
            constexpr size_type rlen = std::min(n1, size() - pos1);
            constexpr int r = traits_type::compare(data() + pos1, s, std::min(rlen, n2));
            if (r == 0) {
                if (rlen < n2) {
                    return -1;
                }
                else if (rlen > n2) {
                    return 1;
                }
            }
            return r;
        }

        constexpr reference
        operator[](size_type i) {
            return elems[i];
        }
        constexpr const_reference
        operator[](size_type i) const {
            return elems[i];
        }
    };

    template <typename T, std::size_t N>
    constexpr basic_string<T, N - 1>
    make_string(const T(&arr)[N]) {
        return basic_string<T, N - 1>(arr);
    }
} // end namespace termcolor2

#endif	// !TERMCOLOR2_STRING_HPP
