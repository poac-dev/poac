#ifndef SEMVER_VECTOR_ITERATOR_TRAITS_HPP
#define SEMVER_VECTOR_ITERATOR_TRAITS_HPP

#include <iterator>
#include <type_traits>

namespace semver {
    template <typename T>
    struct has_iterator_category {
    private:
        struct two {
            char __lx;
            char __lxx;
        };
        template <typename U>
        static two test(...);
        template <typename U>
        static char test(typename U::iterator_category* = 0);
    public:
        static const bool value = sizeof(test<T>(0)) == 1;
    };

    template <typename T, typename U, bool = has_iterator_category<std::iterator_traits<T> >::value>
    struct has_iterator_category_convertible_to
        : public std::integral_constant<bool, std::is_convertible_v<typename std::iterator_traits<T>::iterator_category, U>>
    {};

    template <typename T, typename U>
    struct has_iterator_category_convertible_to<T, U, false>
        : public std::false_type
    {};

    template <typename T>
    struct is_input_iterator
        : public has_iterator_category_convertible_to<T, std::input_iterator_tag>
    {};
    template <typename T>
    constexpr bool is_input_iterator_v = is_input_iterator<T>::value;

    template <typename T>
    struct is_forward_iterator
        : public has_iterator_category_convertible_to<T, std::forward_iterator_tag>
    {};
    template <typename T>
    constexpr bool is_forward_iterator_v = is_forward_iterator<T>::value;
} // end namespace semver

#endif // !SEMVER_VECTOR_ITERATOR_TRAITS_HPP
