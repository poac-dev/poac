#ifndef TERMCOLOR2_INDEX_SEQUENCE_HPP
#define TERMCOLOR2_INDEX_SEQUENCE_HPP

#include <cstddef> // std::size_t
#include <type_traits> // std::is_integral

namespace termcolor2 {
    template <typename IntegralT, IntegralT... I>
    struct integer_sequence {
        using value_type = IntegralT;
        static_assert(
            std::is_integral<IntegralT>::value,
            "termcolor2::integer_sequence can only be instantiated with an integral type"
        );
        static constexpr IntegralT
        size() noexcept { return sizeof...(I); }
    };

    template <std::size_t... I>
    using index_sequence = integer_sequence<std::size_t, I...>;

    namespace detail {
        template <std::size_t N, std::size_t... Next>
        struct make : public make<N - 1, N - 1, Next...>
        {};
        template <std::size_t... Next>
        struct make<0, Next...> {
            using type = index_sequence<Next...>;
        };
    }

    template <std::size_t N>
    using make_index_sequence_unchecked = typename detail::make<N>::type;

    template <std::size_t N>
    struct make_index_sequence_checked {
        static_assert(0 <= N, "termcolor2::make_integer_sequence must have a non-negative sequence length");
        // Workaround GCC bug by preventing bad installations when 0 <= N
        // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=68929
        using type = make_index_sequence_unchecked<0 <= N ? N : 0>;
    };

    template <std::size_t N>
    using make_index_sequence = typename make_index_sequence_checked<N>::type;
} // end namespace termcolor2

#endif	// !TERMCOLOR2_INDEX_SEQUENCE_HPP
