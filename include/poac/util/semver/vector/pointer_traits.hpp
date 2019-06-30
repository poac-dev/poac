// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1006r1.pdf
#ifndef SEMVER_VECTOR_POINTER_TRAITS_HPP
#define SEMVER_VECTOR_POINTER_TRAITS_HPP

#include <cstddef>
#include <memory>
#include <type_traits>

namespace semver {
    template <typename Ptr>
    using pointer_traits_element_type_t = typename std::__pointer_traits_element_type<Ptr>::type;

    template <typename Ptr>
    using pointer_traits_difference_type_t = typename std::__pointer_traits_difference_type<Ptr>::type;

    template <typename Ptr, typename Up>
    using pointer_traits_rebind_t = typename std::__pointer_traits_rebind<Ptr, Up>::type;

    template <typename Ptr>
    struct pointer_traits
    {
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
    struct pointer_traits<T*>
    {
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
} // end namespace semver

#endif // !SEMVER_VECTOR_POINTER_TRAITS_HPP
