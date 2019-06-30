// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1006r1.pdf
#ifndef SEMVER_VECTOR_POINTER_TRAITS_HPP
#define SEMVER_VECTOR_POINTER_TRAITS_HPP

#include <cstddef>
#include <memory>
#include <type_traits> // std::true_type, std::false_type, std::void_t

#include "../config.hpp"

namespace semver {
#ifdef SEMVER_AFTER_CXX14
    template <typename Tp, typename = void>
    struct has_element_type : std::false_type {};

    template <typename Tp>
    struct has_element_type<Tp, std::void_t<typename Tp::element_type>>
            : std::true_type {};

    template <typename Tp, typename Up = void>
    constexpr bool has_element_type_v = has_element_type<Tp, Up>::value;

    template <typename Ptr, bool = has_element_type_v<Ptr>>
    struct pointer_traits_element_type;

    template <typename Ptr>
    struct pointer_traits_element_type<Ptr, true> {
        using type = typename Ptr::element_type;
    };

    template <template <typename, typename...> typename Sp, typename Tp, typename ...Args>
    struct pointer_traits_element_type<Sp<Tp, Args...>, true> {
        using type = typename Sp<Tp, Args...>::element_type;
    };

    template <template <typename, typename...> typename Sp, typename Tp, typename ...Args>
    struct pointer_traits_element_type<Sp<Tp, Args...>, false> {
        using type = Tp;
    };

    template <typename Ptr>
    using pointer_traits_element_type_t = typename pointer_traits_element_type<Ptr>::type;


    template <typename Tp, typename = void>
    struct has_difference_type : std::false_type {};

    template <typename Tp>
    struct has_difference_type<Tp, std::void_t<typename Tp::difference_type>>
            : std::true_type {};

    template <typename Tp, typename Up = void>
    constexpr bool has_difference_type_v = has_difference_type<Tp, Up>::value;

    template <typename Ptr, bool = has_difference_type_v<Ptr>>
    struct pointer_traits_difference_type {
        using type = ptrdiff_t;
    };

    template <typename Ptr>
    struct pointer_traits_difference_type<Ptr, true> {
        using type = typename Ptr::difference_type;
    };

    template <typename Ptr>
    using pointer_traits_difference_type_t = typename pointer_traits_difference_type<Ptr>::type;


    template <typename Tp, typename Up>
    struct has_rebind {
    private:
        struct __two { char __lx; char __lxx; };
        template <class Xp>
        static __two __test(...);
        template <class Xp>
        static char __test(typename Xp::template rebind<Up>* = 0);

    public:
        static const bool value = sizeof(__test<Tp>(0)) == 1;
    };

    template <typename Tp, typename Up>
    constexpr bool has_rebind_v = has_rebind<Tp, Up>::value;

    template <typename Tp, typename Up, bool = has_rebind_v<Tp, Up>>
    struct pointer_traits_rebind {
        using type = typename Tp::template rebind<Up>;
    };

    template <template <typename, typename...> typename Sp, typename Tp, typename ...Args, typename Up>
    struct pointer_traits_rebind<Sp<Tp, Args...>, Up, true> {
        using type = typename Sp<Tp, Args...>::template rebind<Up>;
    };

    template <template <typename, typename...> typename Sp, typename Tp, typename ...Args, typename Up>
    struct pointer_traits_rebind<Sp<Tp, Args...>, Up, false> {
        using type = Sp<Up, Args...>;
    };

    template <typename Ptr, typename Up>
    using pointer_traits_rebind_t = typename pointer_traits_rebind<Ptr, Up>::type;


    template <typename Ptr>
    struct pointer_traits {
        using pointer = Ptr;
        using element_type = pointer_traits_element_type_t<pointer>;
        using difference_type = pointer_traits_difference_type_t<pointer>;

        template <class U>
        using rebind = pointer_traits_rebind_t<pointer, U>;

    private:
        struct nat {};

    public:
        static pointer pointer_to(
                std::conditional_t<std::is_void_v<element_type>, nat, element_type>& r
        ) {
            return pointer::pointer_to(r);
        }
    };

    template <typename T>
    struct pointer_traits<T*> {
        using pointer = T*;
        using element_type = T;
        using difference_type = std::ptrdiff_t;

        template <class U>
        using rebind = U*;

    private:
        struct nat {};

    public:
        static constexpr pointer pointer_to(
                std::conditional_t<std::is_void_v<element_type>, nat, element_type>& r
        ) noexcept {
            return std::addressof(r);
        }
    };

#elif defined(SEMVER_AFTER_CXX17)
    template <typename Ptr>
    using pointer_traits = std::pointer_traits<Ptr>;
#endif
} // end namespace semver

#endif // !SEMVER_VECTOR_POINTER_TRAITS_HPP
