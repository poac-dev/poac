#ifndef POAC_IO_CLI_HPP
#define POAC_IO_CLI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <iomanip>

#include "../util/pretty.hpp"


namespace poac::io::cli {
    namespace detail {
        struct string_raw_construct_t {};
        struct string_checked_construct_t {};

        template <typename CharT, std::size_t N, typename Traits>
        struct basic_string_impl {
            using value_type = typename Traits::char_type;
            using reference = value_type&;
            using const_reference = const value_type&;
            using size_type = std::size_t;
            using difference_type = std::ptrdiff_t;
            using pointer = value_type*;
            using const_pointer = const value_type*;
            using traits_type = Traits;

            value_type elems[N + 1];
            size_type len;

            constexpr basic_string_impl()
                    : elems{}, len()
            {}
            basic_string_impl(const basic_string_impl&) = default;
            basic_string_impl(basic_string_impl&&) = default;

            template <typename String, std::size_t... Indexes>
            constexpr basic_string_impl(
                    std::index_sequence<Indexes...>,
                    detail::string_checked_construct_t,
                    const String& str, size_type pos, size_type n
            )
                    : elems{(
                        Indexes < n
                            ? static_cast<value_type>(str[Indexes + pos])
                            : value_type()
                    )...}
                    , len(
                        N >= n
                            ? n
                            : throw std::length_error("basic_string<>: length exceeded")
                    )
            {}

            template<typename... Args, std::size_t... Indexes>
            constexpr basic_string_impl(
                    std::index_sequence<Indexes...>,
                    detail::string_raw_construct_t,
                    size_type n, Args&&... args
            )
                    : elems{(
                        Indexes < n
                            ? static_cast<value_type>(std::forward<Args>(args))
                            : value_type()
                    )...}
                    , len(n)
            {}
        };
    }

    template<typename String>
    static constexpr int
    compare_impl_1(
            int compared, typename String::size_type n1, typename String::size_type n2
    ) {
        return compared != 0
                   ? compared
                   : n1 < n2
                       ? -1
                       : n2 < n1
                           ? 1
                           : 0
                ;
    }
    template<typename String, typename ConstIterator>
    static constexpr int
    compare_impl(
            typename String::const_iterator data, typename String::size_type pos1, typename String::size_type n1,
            ConstIterator s, typename String::size_type n2
    ) {
        return compare_impl_1<String>(
                String::traits_type::compare(data + pos1, s, std::min(n1, n2)),
                n1, n2
        );
    }

    template <typename CharT, std::size_t N, typename Traits = std::char_traits<CharT>>
    class basic_string {
    private:
        using impl_type = detail::basic_string_impl<CharT, N, Traits>;

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

        impl_type impl_;

        basic_string(const basic_string&) = default;
        explicit constexpr basic_string(const value_type* s)
                : impl_(
                        std::make_index_sequence<N>{},
                        detail::string_checked_construct_t{},
                        s, 0, traits_type::length(s)
                )
        {}

        constexpr basic_string(std::initializer_list<value_type> il)
                : impl_(
                    std::make_index_sequence<N>{},
                    detail::string_checked_construct_t{},
                    il.begin(), 0, il.size()
                )
        {}

        template<typename... Args, typename = typename std::enable_if_t<(sizeof...(Args) <= N)>>
        constexpr basic_string(detail::string_raw_construct_t, size_type n, Args&&... args)
                : impl_(
                    std::make_index_sequence<sizeof...(Args)>{},
                    detail::string_raw_construct_t{},
                    n, std::forward<Args>(args)...
                )
        {}

        constexpr const_pointer
        to_string() const noexcept {
            return data();
        }
        constexpr pointer
        data() noexcept {
            return impl_.elems;
        }
        constexpr const_pointer
        data() const noexcept {
            return impl_.elems;
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
            return impl_.len;
        }

        template<std::size_t N2>
        constexpr int
        compare(basic_string<CharT, N2, Traits> const& str) const {
            return compare(0, size(), str.begin(), str.size());
        }
        constexpr int
        compare(value_type const* s) const {
            return compare(0, size(), s, traits_type::length(s));
        }
        constexpr int
        compare(size_type pos1, size_type n1, value_type const* s, size_type n2) const {
            return size() >= pos1
                   ? compare_impl<basic_string>(begin(), pos1, std::min(n1, size() - pos1), s, n2)
                   : throw std::out_of_range("basic_string<>: index out of range")
                    ;
        }

        // element access:
        constexpr reference
        operator[](size_type i) {
            return impl_.elems[i];
        }
        constexpr const_reference
        operator[](size_type i) const {
            return impl_.elems[i];
        }
    };

    template<typename _CharT, std::size_t N1, std::size_t N2, typename _Traits>
    inline constexpr bool
    operator==(basic_string<_CharT, N1, _Traits> const& lhs, basic_string<_CharT, N2, _Traits> const& rhs) {
        return lhs.compare(rhs) == 0;
    }
    template<typename _CharT, std::size_t N, typename _Traits>
    inline constexpr bool
    operator==(basic_string<_CharT, N, _Traits> const& lhs, _CharT const* rhs) {
        return lhs.compare(rhs) == 0;
    }
    template<typename _CharT, std::size_t N, typename _Traits>
    constexpr bool
    operator==(const _CharT* lhs, const basic_string<_CharT, N, _Traits>& rhs) {
        return 0 == rhs.compare(lhs);
    }
    template<typename _CharT, std::size_t N1, std::size_t N2, typename _Traits>
    constexpr bool
    operator!=(const basic_string<_CharT, N1, _Traits>& lhs, const basic_string<_CharT, N2, _Traits>& rhs) {
        return !(lhs == rhs);
    }
    template<typename _CharT, std::size_t N, typename _Traits>
    constexpr bool
    operator!=(const basic_string<_CharT, N, _Traits>& lhs, const _CharT* rhs) {
        return !(lhs == rhs);
    }
    template<typename _CharT, std::size_t N, typename _Traits>
    constexpr bool
    operator!=(const _CharT* lhs, const basic_string<_CharT, N, _Traits>& rhs) {
        return !(lhs == rhs);
    }


    template <typename T, std::size_t N>
    constexpr basic_string<T, N - 1>
    make_string(const T(&arr)[N]) {
        return basic_string<T, N - 1>(arr);
    }

    template <typename T, typename Tp = T*, std::size_t N = std::extent_v<Tp>>
    constexpr basic_string<T, N>
    make_string(const Tp s) {
        return basic_string<T, N>(s);
    }

    template<class _CharT, std::size_t N, class _Traits>
    std::basic_ostream<_CharT, _Traits>&
    operator<<(std::basic_ostream<_CharT, _Traits>& os,
               const basic_string<_CharT, N, _Traits>& str)
    {
        return std::__put_character_sequence(os, str.data(), str.size());
    }


    namespace detail {
        template<typename T, std::size_t N, typename Traits = std::char_traits<T>>
        class string_construct_access {
        public:
            template<typename... Args>
            static constexpr basic_string<T, N, Traits>
            raw_construct(typename basic_string<T, N, Traits>::size_type n, Args&&... args) {
                return basic_string<T, N, Traits>(detail::string_raw_construct_t{}, n, std::forward<Args>(args)...);
            }
        };

        template<typename T, std::size_t N, typename Traits, std::size_t... Indexes>
        inline constexpr basic_string<T, N + 1, Traits>
        string_concat(
                basic_string<T, N, Traits> const& lhs, std::size_t lsize,
                T const& rhs,
                std::index_sequence<Indexes...>
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
        template<typename T, std::size_t N, typename Traits, std::size_t... Indexes>
        inline constexpr basic_string<T, 1 + N, Traits>
        string_concat(
                T const& lhs,
                basic_string<T, N, Traits> const& rhs, std::size_t rsize,
                std::index_sequence<Indexes...>
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
        template<typename T, std::size_t N, typename Traits, std::size_t M, std::size_t... Indexes>
        inline constexpr basic_string<T, N + (M - 1), Traits>
        string_concat(
                basic_string<T, N, Traits> const& lhs, std::size_t lsize,
                T const (& rhs)[M], std::size_t rsize,
                std::index_sequence<Indexes...>
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
        template<typename T, std::size_t N, typename Traits, std::size_t M, std::size_t... Indexes>
        inline constexpr basic_string<T, (M - 1) + N, Traits>
        string_concat(
                T const (& lhs)[M], std::size_t lsize,
                basic_string<T, N, Traits> const& rhs, std::size_t rsize,
                std::index_sequence<Indexes...>
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
        template<typename T, std::size_t N1, std::size_t N2, typename Traits, std::size_t... Indexes>
        inline constexpr basic_string<T, N1 + N2, Traits>
        string_concat(
                basic_string<T, N1, Traits> const& lhs, std::size_t lsize,
                basic_string<T, N2, Traits> const& rhs, std::size_t rsize,
                std::index_sequence<Indexes...>
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
    }

    template<typename T, std::size_t N, typename Traits>
    inline constexpr basic_string<T, N + 1, Traits>
    operator+(basic_string<T, N, Traits> const& lhs, T const& rhs) {
        return detail::string_concat(
                lhs, lhs.size(),
                rhs,
                std::make_index_sequence<N + 1>{}
        );
    }
    template<typename T, std::size_t N, typename Traits>
    inline constexpr basic_string<T, 1 + N, Traits>
    operator+(T const& lhs, basic_string<T, N, Traits> const& rhs) {
        return detail::string_concat(
                lhs,
                rhs, rhs.size(),
                std::make_index_sequence<1 + N>{}
        );
    }
    template<typename T, std::size_t N, typename Traits, std::size_t M>
    inline constexpr basic_string<T, N + (M - 1), Traits>
    operator+(const basic_string<T, N, Traits>& lhs, const T(&rhs)[M]) {
//        using traits_type = char_traits_helper<Traits>;
        return detail::string_concat(
                lhs, lhs.size(),
                rhs, Traits::length(rhs, M - 1),
                std::make_index_sequence<N + (M - 1)>{}
        );
    }
    template<typename T, std::size_t N, typename Traits, std::size_t M>
    inline constexpr basic_string<T, (M - 1) + N, Traits>
    operator+(const T(&lhs)[M], const basic_string<T, N, Traits>& rhs) {
//        using traits_type = char_traits_helper<Traits>;
        return detail::string_concat(
                lhs, Traits::length(lhs, M - 1),
                rhs, rhs.size(),
                std::make_index_sequence<(M - 1) + N>{}
        );
    }
    template<typename T, std::size_t N1, std::size_t N2, typename Traits>
    inline constexpr basic_string<T, N1 + N2, Traits>
    operator+(basic_string<T, N1, Traits> const& lhs, basic_string<T, N2, Traits> const& rhs) {
        return detail::string_concat(
                lhs, lhs.size(),
                rhs, rhs.size(),
                std::make_index_sequence<N1 + N2>{}
        );
    }

    template <std::size_t N>
    using string = basic_string<char, N>;

    template <std::size_t N>
    using wstring = basic_string<wchar_t, N>;

    template <std::size_t N>
    using u16string = basic_string<char16_t, N>;

    template <std::size_t N>
    using u32string = basic_string<char32_t, N>;

    // Clear screen
    constexpr basic_string hoge = make_string("hoge");
//    constexpr basic_string clr_screen = make_string("\x1b[2J");
//    // Clear from the cursor position to the right end
//    constexpr auto clr_right = to_string("\x1b[0K");
//    // Clear from the cursor position to the left end
//    constexpr auto clr_left = to_string("\x1b[1K");
    // Clear the line at the cursor position
    const std::string clr_line = "\x1b[2K";
//    constexpr auto clr_line = to_string("\x1b[2K");

    inline void set_left(int&& n) {
        std::cout << std::setw(n) << std::left;
    }

    inline namespace literals
    {
        inline namespace color_literals
        {
            namespace preset
            {
                template <typename CharT = char>
                constexpr basic_string red = make_string("\x1b[31m");
                template <>
                constexpr basic_string red<wchar_t> = make_string(L"\x1b[31m");
                template <>
                constexpr basic_string red<char16_t> = make_string(u"\x1b[31m");
                template <>
                constexpr basic_string red<char32_t> = make_string(U"\x1b[31m");

                template <typename CharT = char>
                constexpr basic_string green = make_string("\x1b[32m");
                template <>
                constexpr basic_string green<wchar_t> = make_string(L"\x1b[32m");
                template <>
                constexpr basic_string green<char16_t> = make_string(u"\x1b[32m");
                template <>
                constexpr basic_string green<char32_t> = make_string(U"\x1b[32m");

                template <typename CharT = char>
                constexpr basic_string yellow = make_string("\x1b[33m");
                template <>
                constexpr basic_string yellow<wchar_t> = make_string(L"\x1b[33m");
                template <>
                constexpr basic_string yellow<char16_t> = make_string(u"\x1b[33m");
                template <>
                constexpr basic_string yellow<char32_t> = make_string(U"\x1b[33m");

                template <typename CharT = char>
                constexpr basic_string blue = make_string("\x1b[34m");
                template <>
                constexpr basic_string blue<wchar_t> = make_string(L"\x1b[34m");
                template <>
                constexpr basic_string blue<char16_t> = make_string(u"\x1b[34m");
                template <>
                constexpr basic_string blue<char32_t> = make_string(U"\x1b[34m");

                template <typename CharT = char>
                constexpr basic_string pink = make_string("\x1b[35m");
                template <>
                constexpr basic_string pink<wchar_t> = make_string(L"\x1b[35m");
                template <>
                constexpr basic_string pink<char16_t> = make_string(u"\x1b[35m");
                template <>
                constexpr basic_string pink<char32_t> = make_string(U"\x1b[35m");

                template <typename CharT = char>
                constexpr basic_string gray = make_string("\x1b[90m");
                template <>
                constexpr basic_string gray<wchar_t> = make_string(L"\x1b[90m");
                template <>
                constexpr basic_string gray<char16_t> = make_string(u"\x1b[90m");
                template <>
                constexpr basic_string gray<char32_t> = make_string(U"\x1b[90m");

                template <typename CharT = char>
                constexpr basic_string bold = make_string("\x1b[1m");
                template <>
                constexpr basic_string bold<wchar_t> = make_string(L"\x1b[1m");
                template <>
                constexpr basic_string bold<char16_t> = make_string(u"\x1b[1m");
                template <>
                constexpr basic_string bold<char32_t> = make_string(U"\x1b[1m");

                template <typename CharT = char>
                constexpr basic_string underline = make_string("\x1b[4m");
                template <>
                constexpr basic_string underline<wchar_t> = make_string(L"\x1b[4m");
                template <>
                constexpr basic_string underline<char16_t> = make_string(u"\x1b[4m");
                template <>
                constexpr basic_string underline<char32_t> = make_string(U"\x1b[4m");

                template <typename CharT = char>
                constexpr basic_string reset = make_string("\x1b[0m");
                template <>
                constexpr basic_string reset<wchar_t> = make_string(L"\x1b[0m");
                template <>
                constexpr basic_string reset<char16_t> = make_string(u"\x1b[0m");
                template <>
                constexpr basic_string reset<char32_t> = make_string(U"\x1b[0m");
            }

            namespace detail
            {
                template <std::size_t N, typename CharT, CharT... Str>
                constexpr basic_string<CharT, preset::red<CharT>.size() + N + preset::reset<CharT>.size()>
                red() noexcept
                {
                    return preset::red<CharT> + basic_string<CharT, N>({Str...}) + preset::reset<CharT>;
                }

                template <std::size_t N, typename CharT, CharT... Str>
                constexpr basic_string<CharT, preset::green<CharT>.size() + N + preset::reset<CharT>.size()>
                green() noexcept
                {
                    return preset::green<CharT> + basic_string<CharT, N>({Str...}) + preset::reset<CharT>;
                }

                template <std::size_t N, typename CharT, CharT... Str>
                constexpr basic_string<CharT, preset::yellow<CharT>.size() + N + preset::reset<CharT>.size()>
                yellow() noexcept
                {
                    return preset::yellow<CharT> + basic_string<CharT, N>({Str...}) + preset::reset<CharT>;
                }

                template <std::size_t N, typename CharT, CharT... Str>
                constexpr basic_string<CharT, preset::blue<CharT>.size() + N + preset::reset<CharT>.size()>
                blue() noexcept
                {
                    return preset::blue<CharT> + basic_string<CharT, N>({Str...}) + preset::reset<CharT>;
                }

                template <std::size_t N, typename CharT, CharT... Str>
                constexpr basic_string<CharT, preset::pink<CharT>.size() + N + preset::reset<CharT>.size()>
                pink() noexcept
                {
                    return preset::pink<CharT> + basic_string<CharT, N>({Str...}) + preset::reset<CharT>;
                }

                template <std::size_t N, typename CharT, CharT... Str>
                constexpr basic_string<CharT, preset::gray<CharT>.size() + N + preset::reset<CharT>.size()>
                gray() noexcept
                {
                    return preset::gray<CharT> + basic_string<CharT, N>({Str...}) + preset::reset<CharT>;
                }

                template <std::size_t N, typename CharT, CharT... Str>
                constexpr basic_string<CharT, preset::bold<CharT>.size() + N + preset::reset<CharT>.size()>
                bold() noexcept
                {
                    return preset::bold<CharT> + basic_string<CharT, N>({Str...}) + preset::reset<CharT>;
                }

                template <std::size_t N, typename CharT, CharT... Str>
                constexpr basic_string<CharT, preset::underline<CharT>.size() + N + preset::reset<CharT>.size()>
                underline() noexcept
                {
                    return preset::underline<CharT> + basic_string<CharT, N>({Str...}) + preset::reset<CharT>;
                }
            }

            // -Wno-gnu-string-literal-operator-template
            template <typename CharT, CharT... Str>
            constexpr auto operator "" _red() noexcept
            {
                return detail::red<sizeof...(Str), CharT, Str...>();
            }

            template <typename CharT, CharT... Str>
            constexpr auto operator "" _green() noexcept
            {
                return detail::green<sizeof...(Str), CharT, Str...>();
            }

            template <typename CharT, CharT... Str>
            constexpr auto operator "" _yellow() noexcept
            {
                return detail::yellow<sizeof...(Str), CharT, Str...>();
            }

            template <typename CharT, CharT... Str>
            constexpr auto operator "" _blue() noexcept
            {
                return detail::blue<sizeof...(Str), CharT, Str...>();
            }

            template <typename CharT, CharT... Str>
            constexpr auto operator "" _pink() noexcept
            {
                return detail::pink<sizeof...(Str), CharT, Str...>();
            }

            template <typename CharT, CharT... Str>
            constexpr auto operator "" _gray() noexcept
            {
                return detail::gray<sizeof...(Str), CharT, Str...>();
            }

            template <typename CharT, CharT... Str>
            constexpr auto operator "" _bold() noexcept
            {
                return detail::bold<sizeof...(Str), CharT, Str...>();
            }

            template <typename CharT, CharT... Str>
            constexpr auto operator "" _underline() noexcept
            {
                return detail::underline<sizeof...(Str), CharT, Str...>();
            }
        }
    }

    constexpr basic_string status = "==> "_green;
    constexpr basic_string fetched = "  ●  "_green;
    constexpr basic_string fetch_failed = "  ●  "_red;
//    std::string_view fetch_failed = "  ●  ";

    constexpr basic_string warning = "WARN: "_yellow;
    constexpr basic_string error = "ERROR: "_red;
//    std::string_view error = "ERROR: ";
    constexpr basic_string info = "info: "_blue;
    constexpr basic_string debug_m = "[debug] "_gray;

    inline void status_done() {
        std::cout << status << "Done." << std::endl;
    }


    const std::vector<std::string> spinners{
        "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
    };
    std::string to_spinner(const size_t& index) {
        return spinners[index % spinners.size()];
    }
    const std::vector<std::string> pointers{ ".  ", ".. ", "..." };
    std::string to_pointer(const size_t& index) {
        if      (index <= 2) return pointers[0];
        else if (index <= 5) return pointers[1];
        else                 return pointers[2];
    }

    // Create progress bar, [====>   ]
    std::string to_progress(const int& max_count, int now_count) {
        if (now_count > max_count) {
            now_count = max_count;
        }

        const int bar_size = 50;
        const int percent = (now_count * 100) / max_count;
        const int bar_pos = percent / 2;

        std::string bar = "[";
        if (now_count >= max_count) {
            for (int i = 0; i < (bar_size - 1); ++i) {
                bar += "=";
            }
            bar += ">]";
        } else if ((bar_pos - 1) > 0) {
            for (int i = 0; i < (bar_pos - 1); ++i) {
                bar += "=";
            }
            bar += ">";
            for (int i = 0; i < (bar_size - bar_pos); ++i) {
                bar += " ";
            }
            bar += "]";
        } else if (bar_pos == 1) {
            bar += ">";
            for (int i = 0; i < (bar_size - 1); ++i) {
                bar += " ";
            }
            bar += "]";
        } else {
            for (int i = 0; i < bar_size; ++i) {
                bar += " ";
            }
            bar += "]";
        }
        return bar;
    }
    // Print byte progress bar, [====>   ] 10.21B/21.28KB
    void echo_byte_progress(const int& max_count, const int& now_count) {
        const auto [ parsed_max_byte, max_byte_unit ] = util::pretty::to_byte(max_count);
        const auto [ parsed_now_byte, now_byte_unit ] = util::pretty::to_byte(now_count);
        std::cout << cli::to_progress(max_count, now_count) << " ";
        std::cout << std::fixed;
        std::cout << std::setprecision(2) << parsed_now_byte << now_byte_unit << "/";
        std::cout << std::setprecision(2) << parsed_max_byte << max_byte_unit << std::flush;
    }

    template <typename... T>
    inline void debugln([[maybe_unused]] const T&... s) {
#ifdef DEBUG
        ((std::cout << debug_m) << ... << s) << std::endl;
#endif
    }
    template <typename... T>
    inline void debug([[maybe_unused]] const T &... s) {
#ifdef DEBUG
        ((std::cout << debug_m) << ... << s);
#endif
    }
} // end namespace
#endif // !POAC_IO_CLI_HPP
