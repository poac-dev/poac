#ifndef TERMCOLOR2_CONCAT_HPP
#define TERMCOLOR2_CONCAT_HPP

#include <cstddef> // std::size_t
#include <string> // std::char_traits
#include <utility> // std::forward, std::index_sequence, std::make_index_sequence
#include <poac/util/termcolor2/string.hpp>

namespace termcolor2 {
    namespace detail {
        template <typename T, std::size_t N, typename Traits = std::char_traits<T>>
        struct string_construct_access {
            template <typename... Args>
            static constexpr basic_string<T, N, Traits>
            raw_construct(const typename basic_string<T, N, Traits>::size_type n, Args&&... args) {
                return basic_string<T, N, Traits>(n, std::forward<Args>(args)...);
            }
        };

        template <typename T, std::size_t N, typename Traits, std::size_t... Indexes>
        constexpr basic_string<T, N + 1, Traits>
        string_concat(
            const basic_string<T, N, Traits>& lhs, const std::size_t lsize,
            const T& rhs,
            termcolor2::index_sequence<Indexes...>
        ) {
            using access_type = detail::string_construct_access<T, N + 1, Traits>;
            return access_type::raw_construct(
                    lsize + 1,
                    (Indexes < lsize
                        ? lhs[Indexes]
                        : Indexes < lsize + 1
                            ? rhs
                            : T()
                    )...
            );
        }
        template <typename T, std::size_t N, typename Traits, std::size_t... Indexes>
        constexpr basic_string<T, 1 + N, Traits>
        string_concat(
            const T& lhs,
            const basic_string<T, N, Traits>& rhs, const std::size_t rsize,
            termcolor2::index_sequence<Indexes...>
        ) {
            using access_type = detail::string_construct_access<T, 1 + N, Traits>;
            return access_type::raw_construct(
                    1 + rsize,
                    (Indexes < 1
                     ? lhs
                     : Indexes < 1 + rsize
                       ? rhs[Indexes - 1]
                       : T()
                    )...
            );
        }
        template <typename T, std::size_t N, typename Traits, std::size_t M, std::size_t... Indexes>
        constexpr basic_string<T, N + (M - 1), Traits>
        string_concat(
            const basic_string<T, N, Traits>& lhs, const std::size_t lsize,
            const T(&rhs)[M], const std::size_t rsize,
            termcolor2::index_sequence<Indexes...>
        ) {
            using access_type = detail::string_construct_access<T, N + (M - 1), Traits>;
            return access_type::raw_construct(
                    lsize + rsize,
                    (Indexes < lsize
                        ? lhs[Indexes]
                        : Indexes < lsize + rsize
                            ? rhs[Indexes - lsize]
                            : T()
                    )...
            );
        }
        template <typename T, std::size_t N, typename Traits, std::size_t M, std::size_t... Indexes>
        constexpr basic_string<T, (M - 1) + N, Traits>
        string_concat(
            const T(&lhs)[M], const std::size_t lsize,
            const basic_string<T, N, Traits>& rhs, const std::size_t rsize,
            termcolor2::index_sequence<Indexes...>
        ) {
            using access_type = detail::string_construct_access<T, (M - 1) + N, Traits>;
            return access_type::raw_construct(
                    lsize + rsize,
                    (Indexes < lsize
                        ? *(lhs + Indexes)
                        : Indexes < lsize + rsize
                            ? rhs[Indexes - lsize]
                            : T()
                    )...
            );
        }
        template <typename T, std::size_t N1, std::size_t N2, typename Traits, std::size_t... Indexes>
        constexpr basic_string<T, N1 + N2, Traits>
        string_concat(
            const basic_string<T, N1, Traits>& lhs, const std::size_t lsize,
            const basic_string<T, N2, Traits>& rhs, const std::size_t rsize,
            termcolor2::index_sequence<Indexes...>
        ) {
            using access_type = detail::string_construct_access<T, N1 + N2, Traits>;
            return access_type::raw_construct(
                    lsize + rsize,
                    (Indexes < lsize
                        ? lhs[Indexes]
                        : Indexes < lsize + rsize
                            ? rhs[Indexes - lsize]
                            : T()
                    )...
            );
        }
    } // end namespace detail

    template <typename T, std::size_t N, typename Traits>
    constexpr basic_string<T, N + 1, Traits>
    operator+(const basic_string<T, N, Traits>& lhs, const T& rhs) {
        return detail::string_concat(
            lhs, lhs.size(),
            rhs,
            termcolor2::make_index_sequence<N + 1>{}
        );
    }
    template <typename T, std::size_t N, typename Traits>
    constexpr basic_string<T, 1 + N, Traits>
    operator+(const T& lhs, const basic_string<T, N, Traits>& rhs) {
        return detail::string_concat(
            lhs,
            rhs, rhs.size(),
            termcolor2::make_index_sequence<1 + N>{}
        );
    }
    template <typename T, std::size_t N, typename Traits, std::size_t M>
    constexpr basic_string<T, N + (M - 1), Traits>
    operator+(const basic_string<T, N, Traits>& lhs, const T(&rhs)[M]) {
        return detail::string_concat(
            lhs, lhs.size(),
            rhs, M - 1,
            termcolor2::make_index_sequence<N + (M - 1)>{}
        );
    }
    template <typename T, std::size_t N, typename Traits, std::size_t M>
    constexpr basic_string<T, (M - 1) + N, Traits>
    operator+(const T(&lhs)[M], const basic_string<T, N, Traits>& rhs) {
        return detail::string_concat(
            lhs, M - 1,
            rhs, rhs.size(),
            termcolor2::make_index_sequence<(M - 1) + N>{}
        );
    }
    template <typename T, std::size_t N1, std::size_t N2, typename Traits>
    constexpr basic_string<T, N1 + N2, Traits>
    operator+(const basic_string<T, N1, Traits>& lhs, const basic_string<T, N2, Traits>& rhs) {
        return detail::string_concat(
            lhs, lhs.size(),
            rhs, rhs.size(),
            termcolor2::make_index_sequence<N1 + N2>{}
        );
    }
} // end namespace termcolor2

#endif	// !TERMCOLOR2_CONCAT_HPP
