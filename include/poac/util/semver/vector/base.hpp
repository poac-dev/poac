// http://open-std.org/JTC1/SC22/WG21/docs/papers/2018/p1004r1.pdf
#ifndef SEMVER_VECTOR_BASE_HPP
#define SEMVER_VECTOR_BASE_HPP

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

namespace semver {
    template <class Tp, class Allocator>
    class vector_base
    {
    public:
        using allocator_type = Allocator;
        using alloc_traits_type = std::allocator_traits<allocator_type>;
        using size_type = typename alloc_traits_type::size_type;

    protected:
        using value_type = Tp;
        using reference = value_type&;
        using const_reference = const value_type&;
        using difference_type = typename alloc_traits_type::difference_type;
        using pointer = typename alloc_traits_type::pointer;
        using const_pointer = typename alloc_traits_type::const_pointer;
        using iterator = pointer;
        using const_iterator = const_pointer;

        pointer m_begin;
        pointer m_end;
        std::pair<pointer, allocator_type> m_end_cap;

        constexpr allocator_type&
        alloc() noexcept {
            return m_end_cap.second;
        }
        constexpr allocator_type&
        alloc() const noexcept {
            return m_end_cap.second;
        }

        constexpr pointer&
        end_cap() noexcept {
            return m_end_cap.first;
        }
        constexpr pointer&
        end_cap() const noexcept {
            return m_end_cap.first;
        }

        constexpr vector_base()
        noexcept(std::is_nothrow_default_constructible<allocator_type>::value)
            : m_begin(nullptr)
            , m_end(nullptr)
            , m_end_cap(nullptr)
        {}

        constexpr explicit vector_base(const allocator_type& a)
            : m_begin(nullptr)
            , m_end(nullptr)
            , m_end_cap(nullptr, a)
        {}

        constexpr explicit vector_base(allocator_type&& a) noexcept
            : m_begin(nullptr)
            , m_end(nullptr)
            , m_end_cap(nullptr, std::move(a))
        {}

        ~vector_base() {
            if (m_begin != nullptr) {
                clear();
                alloc_traits_type::deallocate(alloc(), m_begin, capacity());
            }
        }

        constexpr void
        clear() noexcept {
            destruct_at_end(m_begin);
        }

        constexpr size_type
        capacity() const noexcept {
            return static_cast<size_type>(end_cap() - m_begin);
        }

        constexpr void
        destruct_at_end(pointer new_last) noexcept {
            pointer soon_to_be_end = m_end;
            while (new_last != soon_to_be_end) {
                alloc_traits_type::destroy(alloc(), --soon_to_be_end);
            }
            m_end = new_last;
        }

        constexpr void
        copy_assign_alloc(const vector_base& c) {
            copy_assign_alloc(c, std::integral_constant<bool,
                    alloc_traits_type::propagate_on_container_copy_assignment::value>());
        }

        constexpr void
        move_assign_alloc(vector_base& c)
        noexcept(!alloc_traits_type::propagate_on_container_move_assignment::value ||
                std::is_nothrow_move_assignable<allocator_type>::value
        ) {
            move_assign_alloc(c, std::integral_constant<bool,
                    alloc_traits_type::propagate_on_container_move_assignment::value>());
        }

    private:
        constexpr void
        copy_assign_alloc(const vector_base& c, std::true_type) {
            if (alloc() != c.alloc()) {
                clear();
                alloc_traits_type::deallocate(alloc(), m_begin, capacity());
                m_begin = m_end = end_cap() = nullptr;
            }
            alloc() = c.alloc();
        }

        constexpr void
        copy_assign_alloc(const vector_base&, std::false_type)
        {}

        constexpr void
        move_assign_alloc(vector_base& c, std::true_type)
        noexcept(std::is_nothrow_move_assignable<allocator_type>::value) {
            alloc() = std::move(c.alloc());
        }

        constexpr void
        move_assign_alloc(vector_base&, std::false_type)
        noexcept
        {}
    };
} // end namespace semver

#endif // !SEMVER_VECTOR_BASE_HPP
