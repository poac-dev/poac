#ifndef POAC_IO_CLI_HPP
#define POAC_IO_CLI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <iomanip>

#include "../util/pretty.hpp"


namespace poac::io::cli {
    // Clear screen
    const std::string clr_screen = "\x1b[2J";
    // Clear from the cursor position to the right end
    const std::string clr_right = "\x1b[0K";
    // Clear from the cursor position to the left end
    const std::string clr_left = "\x1b[1K";
    // Clear the line at the cursor position
    const std::string clr_line = "\x1b[2K";
    // Move cursor position
    inline std::string up(unsigned int y) {
        return "\x1b[" + std::to_string(y) + "A";
    }
    inline std::string down(unsigned int y) {
        return "\x1b[" + std::to_string(y) + "B";
    }
    inline std::string right(unsigned int x) {
        return "\x1b[" + std::to_string(x) + "C";
    }
    inline std::string left(unsigned int x) {
        return "\x1b[" + std::to_string(x) + "D";
    }

    inline void set_left(int&& n) {
        std::cout << std::setw(n) << std::left;
    }

    inline namespace literals
    {
        inline namespace color_literals
        {
            namespace preset
            {
                template <typename T = char>
                const std::basic_string<T> red = reinterpret_cast<const T*>("\x1b[31m");

                template <typename T = char>
                const std::basic_string<T> green = reinterpret_cast<const T*>("\x1b[32m");

                template <typename T = char>
                const std::basic_string<T> yellow = reinterpret_cast<const T*>("\x1b[33m");

                template <typename T = char>
                const std::basic_string<T> blue = reinterpret_cast<const T*>("\x1b[34m");

                template <typename T = char>
                const std::basic_string<T> pink = reinterpret_cast<const T*>("\x1b[35m");

                template <typename T = char>
                const std::basic_string<T> gray = reinterpret_cast<const T*>("\x1b[90m");

                template <typename T = char>
                const std::basic_string<T> bold = reinterpret_cast<const T*>("\x1b[1m");

                template <typename T = char>
                const std::basic_string<T> underline = reinterpret_cast<const T*>("\x1b[4m");

                template <typename T = char>
                const std::basic_string<T> reset = reinterpret_cast<const T*>("\x1b[0m");
            }

            namespace detail
            {
                template <typename T>
                inline std::basic_string<T> red(const T* __str, size_t __len)
                {
                    return preset::red<T> + std::basic_string<T>(__str, __len) + preset::reset<T>;
                }

                template <typename T>
                inline std::basic_string<T> green(const T* __str, size_t __len)
                {
                    return preset::green<T> + std::basic_string<T>(__str, __len) + preset::reset<T>;
                }

                template <typename T>
                inline std::basic_string<T> yellow(const T* __str, size_t __len)
                {
                    return preset::yellow<T> + std::basic_string<T>(__str, __len) + preset::reset<T>;
                }

                template <typename T>
                inline std::basic_string<T> blue(const T* __str, size_t __len)
                {
                    return preset::blue<T> + std::basic_string<T>(__str, __len) + preset::reset<T>;
                }

                template <typename T>
                inline std::basic_string<T> pink(const T* __str, size_t __len)
                {
                    return preset::pink<T> + std::basic_string<T>(__str, __len) + preset::reset<T>;
                }

                template <typename T>
                inline std::basic_string<T> gray(const T* __str, size_t __len)
                {
                    return preset::gray<T> + std::basic_string<T>(__str, __len) + preset::reset<T>;
                }

                template <typename T>
                inline std::basic_string<T> bold(const T* __str, size_t __len)
                {
                    return preset::bold<T> + std::basic_string<T>(__str, __len) + preset::reset<T>;
                }

                template <typename T>
                inline std::basic_string<T> underline(const T* __str, size_t __len)
                {
                    return preset::underline<T> + std::basic_string<T>(__str, __len) + preset::reset<T>;
                }
            }

            inline std::string operator "" _red(const char* __str, size_t __len)
            {
                return detail::red(__str, __len);
            }
            inline std::wstring operator "" _red(const wchar_t* __str, size_t __len)
            {
                return detail::red(__str, __len);
            }
            inline std::u16string operator "" _red(const char16_t* __str, size_t __len)
            {
                return detail::red(__str, __len);
            }
            inline std::u32string operator "" _red(const char32_t* __str, size_t __len)
            {
                return detail::red(__str, __len);
            }

            inline std::string operator "" _green(const char* __str, size_t __len)
            {
                return detail::green(__str, __len);
            }
            inline std::wstring operator "" _green(const wchar_t* __str, size_t __len)
            {
                return detail::green(__str, __len);
            }
            inline std::u16string operator "" _green(const char16_t* __str, size_t __len)
            {
                return detail::green(__str, __len);
            }
            inline std::u32string operator "" _green(const char32_t* __str, size_t __len)
            {
                return detail::green(__str, __len);
            }

            inline std::string operator "" _yellow(const char* __str, size_t __len)
            {
                return detail::yellow(__str, __len);
            }
            inline std::wstring operator "" _yellow(const wchar_t* __str, size_t __len)
            {
                return detail::yellow(__str, __len);
            }
            inline std::u16string operator "" _yellow(const char16_t* __str, size_t __len)
            {
                return detail::yellow(__str, __len);
            }
            inline std::u32string operator "" _yellow(const char32_t* __str, size_t __len)
            {
                return detail::yellow(__str, __len);
            }

            inline std::string operator "" _blue(const char* __str, size_t __len)
            {
                return detail::blue(__str, __len);
            }
            inline std::wstring operator "" _blue(const wchar_t* __str, size_t __len)
            {
                return detail::blue(__str, __len);
            }
            inline std::u16string operator "" _blue(const char16_t* __str, size_t __len)
            {
                return detail::blue(__str, __len);
            }
            inline std::u32string operator "" _blue(const char32_t* __str, size_t __len)
            {
                return detail::blue(__str, __len);
            }

            inline std::string operator "" _pink(const char* __str, size_t __len)
            {
                return detail::pink(__str, __len);
            }
            inline std::wstring operator "" _pink(const wchar_t* __str, size_t __len)
            {
                return detail::pink(__str, __len);
            }
            inline std::u16string operator "" _pink(const char16_t* __str, size_t __len)
            {
                return detail::pink(__str, __len);
            }
            inline std::u32string operator "" _pink(const char32_t* __str, size_t __len)
            {
                return detail::pink(__str, __len);
            }

            inline std::string operator "" _gray(const char* __str, size_t __len)
            {
                return detail::gray(__str, __len);
            }
            inline std::wstring operator "" _gray(const wchar_t* __str, size_t __len)
            {
                return detail::gray(__str, __len);
            }
            inline std::u16string operator "" _gray(const char16_t* __str, size_t __len)
            {
                return detail::gray(__str, __len);
            }
            inline std::u32string operator "" _gray(const char32_t* __str, size_t __len)
            {
                return detail::gray(__str, __len);
            }

            inline std::string operator "" _bold(const char* __str, size_t __len)
            {
                return detail::bold(__str, __len);
            }
            inline std::wstring operator "" _bold(const wchar_t* __str, size_t __len)
            {
                return detail::bold(__str, __len);
            }
            inline std::u16string operator "" _bold(const char16_t* __str, size_t __len)
            {
                return detail::bold(__str, __len);
            }
            inline std::u32string operator "" _bold(const char32_t* __str, size_t __len)
            {
                return detail::bold(__str, __len);
            }

            inline std::string operator "" _underline(const char* __str, size_t __len)
            {
                return detail::underline(__str, __len);
            }
            inline std::wstring operator "" _underline(const wchar_t* __str, size_t __len)
            {
                return detail::underline(__str, __len);
            }
            inline std::u16string operator "" _underline(const char16_t* __str, size_t __len)
            {
                return detail::underline(__str, __len);
            }
            inline std::u32string operator "" _underline(const char32_t* __str, size_t __len)
            {
                return detail::underline(__str, __len);
            }
        }
    }


    const std::string status = "==> "_green;
    const std::string fetched = "  ●  "_green;
    const std::string fetch_failed = "  ●  "_red;

    const std::string warning = "WARN: "_yellow;
    const std::string error = "ERROR: "_red;
    const std::string info = "info: "_blue;
    const std::string debug_m = "[debug] "_gray;

    inline std::string to_status(const std::string& s) { return status + s; }
    inline std::string to_fetched(const std::string& s) { return fetched + s; }
    inline std::string to_fetch_failed(const std::string& s) { return fetch_failed + s; }

    inline std::string to_info(const std::string& s) { return info + s; }
    inline std::string to_warning(const std::string& s) { return warning + s; }
    inline std::string to_error(const std::string& s) { return error + s; }

    inline std::string status_done() { return to_status("Done."); }


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
        }
        else if ((bar_pos - 1) > 0) {
            for (int i = 0; i < (bar_pos - 1); ++i) {
                bar += "=";
            }
            bar += ">";
            for (int i = 0; i < (bar_size - bar_pos); ++i) {
                bar += " ";
            }
            bar += "]";
        }
        else if (bar_pos == 1) {
            bar += ">";
            for (int i = 0; i < (bar_size - 1); ++i) {
                bar += " ";
            }
            bar += "]";
        }
        else {
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
    inline void println(const T&... s) {
        (std::cout << ... << s) << std::endl;
    }
    template <typename... T>
    inline void print(const T&... s) {
        (std::cout << ... << s);
    }

    template <typename... T>
    inline void debugln([[maybe_unused]] const T&... s) {
#ifdef DEBUG
        println(debug_m, s...);
#endif
    }
    template <typename... T>
    inline void debug([[maybe_unused]] const T &... s) {
#ifdef DEBUG
        print(debug_m, s...);
#endif
    }
} // end namespace
#endif // !POAC_IO_CLI_HPP
