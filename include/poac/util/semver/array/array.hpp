#ifndef SEMVER_ARRAY_ARRAY_HPP
#define SEMVER_ARRAY_ARRAY_HPP

#include <cstddef> // std::size_t
#include <iterator> // std::reverse_iterator
#include <utility> // std::move

namespace semver {
    namespace detail {
        template <typename InputIterator, typename OutputIterator>
        constexpr OutputIterator
        copy(InputIterator first, InputIterator last, OutputIterator target) {
            while (first != last) {
                *target++ = *first++;
            }
            return target;
        }

        template <typename InputIterator, typename OutputIterator>
        constexpr OutputIterator
        move(InputIterator first, InputIterator last, OutputIterator target) {
            while (first != last) {
                *target++ = std::move(*first++);
            }
            return target;
        }
    }

    template <typename T, std::size_t N>
    struct array {
        using value_type = T;
        using reference = value_type&;
        using const_reference = const value_type&;
        using iterator = value_type*;
        using const_iterator = const value_type*;
        using pointer = value_type*;
        using const_pointer = const value_type*;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        value_type elems[N ? N : 1];

        //
        // construct/copy/destroy:
        //
        template<typename T2>
        constexpr array& operator=(array<T2, N> const& rhs) {
            detail::copy(rhs.begin(), rhs.end(), begin());
            return *this;
        }
        template<typename T2>
        constexpr array& operator=(array<T2, N>&& rhs) {
            detail::move(rhs.begin(), rhs.end(), begin());
            return *this;
        }

        //
        // iterators:
        //
        constexpr iterator
        begin() noexcept {
            return iterator(elems);
        }
        constexpr const_iterator
        begin() const noexcept {
            return iterator(elems);
        }
        constexpr iterator
        end() noexcept {
            return iterator(elems) + size();
        }
        constexpr const_iterator
        end() const noexcept {
            return iterator(elems) + size();
        }

        constexpr reverse_iterator
        rbegin() noexcept {
            return reverse_iterator(end());
        }
        constexpr const_reverse_iterator
        rbegin() const noexcept {
            return const_reverse_iterator(end());
        }
        constexpr reverse_iterator
        rend() noexcept {
            return reverse_iterator(begin());
        }
        constexpr const_reverse_iterator
        rend() const noexcept {
            return const_reverse_iterator(begin());
        }

        constexpr const_iterator
        cbegin() const noexcept {
            return const_iterator(elems);
        }

        constexpr const_iterator
        cend() const noexcept {
            return const_iterator(elems) + size();
        }

        constexpr const_reverse_iterator
        crbegin() const noexcept {
            return const_reverse_iterator(end());
        }
        constexpr const_reverse_iterator
        crend() const noexcept {
            return const_reverse_iterator(begin());
        }

        //
        // capacity:
        //
        constexpr size_type
        size() const noexcept {
            return N;
        }

        constexpr size_type
        max_size() const noexcept {
            return size();
        }

        constexpr bool
        empty() const noexcept {
            return size() == 0;
        }

        //
        // element access:
        //
        constexpr reference
        operator[](size_type i) {
            return elems[i];
        }
        constexpr const_reference
        operator[](size_type i) const {
            return elems[i];
        }

        constexpr reference
        at(size_type i) {
            if (i >= size()) {
                throw std::out_of_range("semver::array");
            }
            return elems[i];
        }
        constexpr const_reference
        at(size_type i) const {
            if (i >= size()) {
                throw std::out_of_range("semver::array");
            }
            return elems[i];
        }

        constexpr reference
        front() {
            return elems[0];
        }
        constexpr const_reference
        front() const {
            return elems[0];
        }
        constexpr reference
        back() {
            return elems[size() - 1];
        }
        constexpr const_reference
        back() const {
            return elems[size() - 1];
        }

        constexpr pointer
        data() noexcept {
            return elems;
        }
        constexpr const_pointer
        data() const noexcept {
            return elems;
        }

        //
//        constexpr void
//        fill(const value_type& __u) {
//            _VSTD::fill_n(__elems_, _Size, __u);
//        }
//
//        constexpr void
//        swap(array& __a) noexcept(__is_nothrow_swappable<_Tp>::value) {
//            std::swap_ranges(__elems_, __elems_ + _Size, __a.__elems_);
//        }
    };
} // end namespace semver

#endif // !SEMVER_ARRAY_ARRAY_HPP
