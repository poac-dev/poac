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
    std::string up(unsigned int y)    { return "\x1b["+std::to_string(y)+"A"; }
    std::string down(unsigned int y)  { return "\x1b["+std::to_string(y)+"B"; }
    std::string right(unsigned int x) { return "\x1b["+std::to_string(x)+"C"; }
    std::string left(unsigned int x)  { return "\x1b["+std::to_string(x)+"D"; }
    std::string location(unsigned int x, unsigned int y) {
        return "\x1b["+std::to_string(x)+";"+std::to_string(y)+"H";
    }

    void rel_mv(int x=0, int y=0) {
        if (x > 0) std::cout << right(x);
        else       std::cout << left(-x);
        if (y > 0) std::cout << up(y);
        else       std::cout << down(-y);
    }

    void set_left(int&& n) { std::cout << std::setw(n) << std::left; }
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

    void echo() { std::cout << std::endl; }
    void echo(const std::string& s) { std::cout << s << std::endl; }

    const std::string red = "\x1b[31m";
    const std::string green = "\x1b[32m";
    const std::string yellow = "\x1b[33m";
    const std::string blue = "\x1b[34m";
    const std::string bold = "\x1b[1m";
    const std::string underline = "\x1b[4m";
    const std::string reset = "\x1b[0m";

    std::string to_red(const std::string& s) { return red+s+reset; }
    std::string to_green(const std::string& s) { return green+s+reset; }
    std::string to_yellow(const std::string& s) { return yellow+s+reset; }
    std::string to_blue(const std::string& s) { return blue+s+reset; }
    std::string to_bold(const std::string& s) { return bold+s+reset; }
    std::string to_underline(const std::string& s) { return underline+s+reset; }


    const std::string status = to_green("==> ");
    const std::string fetched = to_green("  ●  ");
    const std::string fetch_failed = to_red("  ●  ");

    std::string to_status(const std::string& s) { return status+s; }
    std::string to_fetched(const std::string& s) { return fetched+s; }
    std::string to_fetch_failed(const std::string& s) { return fetch_failed+s; }

    std::string status_done() { return to_status("Done."); }

    void debug(const std::string& s) { if (DEBUG_FLAG) echo(s); }


    const std::vector<std::string> spinners{
        "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"
    };
    std::string at_spinner(const size_t& index) {
        return spinners[index % spinners.size()];
    }
    const std::vector<std::string> pointers{ ".  ", ".. ", "..." };
    std::string at_pointer(const size_t& index) {
        if      (index <= 2) return pointers[0];
        else if (index <= 5) return pointers[1];
        else                 return pointers[2];
    }
} // end namespace
#endif // !POAC_IO_CLI_HPP
