#ifndef SEMVER_UTILITY_SWAP_HPP
#define SEMVER_UTILITY_SWAP_HPP

#include <type_traits> // std::enable_if, std::is_move_constructible, std::is_move_assignable, std::is_nothrow_move_constructible, std::is_nothrow_move_assignable
#include <utility> // std::move

namespace semver {
    template <typename T>
    constexpr std::enable_if_t<
            std::is_move_constructible_v<T>
            && std::is_move_assignable_v<T>>
    swap(T& a, T& b)
    noexcept(std::is_nothrow_move_constructible_v<T>
            && std::is_nothrow_move_assignable_v<T>)
    {
        T temp = std::move(a);
        a = std::move(b);
        b = std::move(temp);
    }
} // end namespace semver

#endif // !SEMVER_UTILITY_SWAP_HPP
