#ifndef POAC_IO_CLI_HPP
#define POAC_IO_CLI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <iomanip>


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
    inline std::string up(unsigned int y)    { return "\x1b["+std::to_string(y)+"A"; }
    inline std::string down(unsigned int y)  { return "\x1b["+std::to_string(y)+"B"; }
    inline std::string right(unsigned int x) { return "\x1b["+std::to_string(x)+"C"; }
    inline std::string left(unsigned int x)  { return "\x1b["+std::to_string(x)+"D"; }
    inline std::string location(unsigned int x, unsigned int y) {
        return "\x1b["+std::to_string(x)+";"+std::to_string(y)+"H";
    }

    inline void rel_mv(int x=0, int y=0) {
        if (x > 0) std::cout << right(static_cast<unsigned int>(x));
        else       std::cout << left(static_cast<unsigned int>(-1 * x));
        if (y > 0) std::cout << up(static_cast<unsigned int>(y));
        else       std::cout << down(static_cast<unsigned int>(-1 * y));
    }

    inline void set_left(int&& n) { std::cout << std::setw(n) << std::left; }
    std::string set_left(const std::string& s, int&& n) {
        if (const int diff = n - static_cast<int>(s.size()); diff > 0) {
            std::string ret = s;
            for (int i = 0; i < diff; ++i)
                ret += " ";
            return ret;
        }
        else {
            return s;
        }
    }

    const std::string red = "\x1b[31m";
    const std::string green = "\x1b[32m";
    const std::string yellow = "\x1b[33m";
    const std::string blue = "\x1b[34m";
    const std::string pink = "\x1b[35m";
    const std::string lightblue = "\x1b[36m";
    const std::string gray = "\x1b[90m";
    const std::string bold = "\x1b[1m";
    const std::string underline = "\x1b[4m";
    const std::string reset = "\x1b[0m";

    inline std::string to_red(const std::string& s) { return red+s+reset; }
    inline std::string to_green(const std::string& s) { return green+s+reset; }
    inline std::string to_yellow(const std::string& s) { return yellow+s+reset; }
    inline std::string to_blue(const std::string& s) { return blue+s+reset; }
    inline std::string to_pink(const std::string& s) { return pink+s+reset; }
    inline std::string to_lightblue(const std::string& s) { return lightblue+s+reset; }
    inline std::string to_gray(const std::string& s) { return gray+s+reset; }
    inline std::string to_bold(const std::string& s) { return bold+s+reset; }
    inline std::string to_underline(const std::string& s) { return underline+s+reset; }


    const std::string status = to_green("==> ");
    const std::string fetched = to_green("  ●  ");
    const std::string fetch_failed = to_red("  ●  ");

    const std::string info = to_gray("INFO: ");
    const std::string warning = to_yellow("WARN: ");
    const std::string error = to_red("ERROR: ");

    inline std::string to_status(const std::string& s) { return status+s; }
    inline std::string to_fetched(const std::string& s) { return fetched+s; }
    inline std::string to_fetch_failed(const std::string& s) { return fetch_failed+s; }

    inline std::string to_info(const std::string& s) { return info+s; }
    inline std::string to_warning(const std::string& s) { return warning+s; }
    inline std::string to_error(const std::string& s) { return error+s; }

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
    std::string to_progress(const int now_count, const int max_count) {
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
        }
        else if (bar_pos == 1) {
            bar += ">";
            for (int i = 0; i < (bar_size - 1); ++i) {
                bar += " ";
            }
        }
        else {
            for (int i = 0; i < bar_size; ++i) {
                bar += " ";
            }
        }
        return bar;
    }


    template <typename... T>
    inline void echo(const T&... s) {
        (std::cout << ... << s) << std::endl;
    }
    template <typename... T>
    inline void echo_noln(const T&... s) {
        (std::cout << ... << s);
    }

    template <typename... T>
    inline void debugln([[maybe_unused]] const T&... s) {
#ifdef DEBUG
        echo(to_gray("[debug] "), s...);
#endif
    }
    template <typename... T>
    inline void debug([[maybe_unused]] const T &... s) {
#ifdef DEBUG
        echo_noln(to_gray("[debug] "), s...);
#endif
    }
} // end namespace
#endif // !POAC_IO_CLI_HPP
