#ifndef TERMCOLOR2_STRING_HPP
#define TERMCOLOR2_STRING_HPP

#include <cstddef> // std::size_t
#include <algorithm> // std::min
#include <stdexcept> // std::length_error, std::out_of_range
#include <iterator> // std::reverse_iterator
#include <utility> // std::index_sequence, std::make_index_sequence, std::forward
#include <poac/util/termcolor2/char_traits.hpp>

namespace termcolor2 {
    template <typename CharT, std::size_t N, typename Traits = termcolor2::char_traits<CharT>>
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
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    private:
        value_type elems[N + 1];
        size_type len;

    public:
        basic_string(const basic_string&) = default;
        basic_string(basic_string &&) noexcept = default;
        basic_string& operator=(const basic_string&) = default;
        basic_string& operator=(basic_string&&) noexcept = default;
        ~basic_string() = default;

        constexpr basic_string()
                : elems{}, len() {}

        template <typename Str, std::size_t... Indexes>
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

        template <typename... Args, std::size_t... Indexes>
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

        explicit constexpr basic_string(std::initializer_list<value_type> il)
            : basic_string(
                  std::make_index_sequence<N>{},
                  il.begin(), 0, il.size()
              ) // delegation
        {}

        template <typename... Args, typename = typename std::enable_if<(sizeof...(Args) <= N)>::type>
        explicit constexpr basic_string(size_type n, Args&&... args)
            : basic_string(
                  std::make_index_sequence<sizeof...(Args)>{},
                  n, std::forward<Args>(args)...
              ) // delegation
        {}

        std::basic_string<CharT>
        to_string() const noexcept {
            return c_str();
        }

        constexpr const_pointer
        c_str() const noexcept {
            return data();
        }
        TERMCOLOR2_CXX14_CONSTEXPR pointer
        data() noexcept {
            return elems;
        }
        constexpr const_pointer
        data() const noexcept {
            return elems;
        }

        TERMCOLOR2_CXX14_CONSTEXPR iterator
        begin() noexcept {
            return data();
        }
        constexpr const_iterator
        begin() const noexcept {
            return data();
        }
        TERMCOLOR2_CXX14_CONSTEXPR iterator
        end() noexcept {
            return data() + size();
        }
        constexpr const_iterator
        end() const noexcept {
            return data() + size();
        }

        constexpr const_iterator
        cbegin() const noexcept {
            return data();
        }
        constexpr const_iterator
        cend() const noexcept {
            return data() + size();
        }

        TERMCOLOR2_CXX14_CONSTEXPR reverse_iterator
        rbegin() noexcept {
            return const_reverse_iterator(end());
        }
        TERMCOLOR2_CXX14_CONSTEXPR const_reverse_iterator
        rbegin() const noexcept {
            return const_reverse_iterator(end());
        }
        TERMCOLOR2_CXX14_CONSTEXPR reverse_iterator
        rend() noexcept {
            return const_reverse_iterator(begin());
        }
        TERMCOLOR2_CXX14_CONSTEXPR const_reverse_iterator
        rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        TERMCOLOR2_CXX14_CONSTEXPR const_reverse_iterator
        crbegin() const noexcept {
            return const_reverse_iterator(end());
        }
        TERMCOLOR2_CXX14_CONSTEXPR const_reverse_iterator
        crend() const noexcept {
            return const_reverse_iterator(begin());
        }

        constexpr size_type
        size() const noexcept {
            return len;
        }
        constexpr size_type
        length() const noexcept {
            return size();
        }

        constexpr size_type
        length(const value_type* str) const noexcept {
            return traits_type::length(str);
        }

        [[nodiscard]] constexpr bool
        empty() const noexcept {
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
            return size() < pos1
                   ? throw std::out_of_range("termcolor2::basic_string")
                   : compare_impl(pos1, std::min(n1, size() - pos1), s, n2);
        }
    private:
        constexpr int
        compare_impl(size_type pos1, const size_type rlen, const value_type* s, size_type n2) const {
            return compare_impl(traits_type::compare(data() + pos1, s, std::min(rlen, n2)), rlen, n2);
        }
        constexpr int
        compare_impl(const int r, const size_type rlen, size_type n2) const {
            return r == 0
                   ? rlen < n2
                     ? -1
                     : rlen > n2
                       ? 1
                       : r
                   : r;
        }

    public:
        TERMCOLOR2_CXX14_CONSTEXPR reference
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
