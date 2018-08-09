#ifndef POAC_IO_CLI_HPP
#define POAC_IO_CLI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <cstdio>
#include <iomanip>


namespace poac::io::cli {
    // Clear screen
    static constexpr std::string_view clr_screen = "\x1b[2J";
    // Clear from the cursor position to the right end
    static constexpr std::string_view clr_right = "\x1b[0K";
    // Clear from the cursor position to the left end
    static constexpr std::string_view clr_left = "\x1b[1K";
    // Clear the line at the cursor position
    static constexpr std::string_view clr_line = "\x1b[2K";
    // Move cursor position
    const std::string_view up(unsigned int y)    { return "\x1b["+std::to_string(y)+"A"; }
    const std::string_view down(unsigned int y)  { return "\x1b["+std::to_string(y)+"B"; }
    const std::string_view right(unsigned int x) { return "\x1b["+std::to_string(x)+"C"; }
    const std::string_view left(unsigned int x)  { return "\x1b["+std::to_string(x)+"D"; }
    const std::string_view location(unsigned int x, unsigned int y) {
        return "\x1b["+std::to_string(x)+";"+std::to_string(y)+"H";
    }

    void rel_mv(int x=0, int y=0) {
        if (x > 0) std::cout << right(x);
        else       std::cout << left(-x);
        if (y > 0) std::cout << up(y);
        else       std::cout << down(-y);
    }

    void set_left(const int&& n) { std::cout << std::setw(n) << std::left; }

    static constexpr std::string_view red = "\x1b[31m";
    static constexpr std::string_view green = "\x1b[32m";
    static constexpr std::string_view yellow = "\x1b[33m";
    static constexpr std::string_view blue = "\x1b[34m";
    static constexpr std::string_view bold = "\x1b[1m";
    static constexpr std::string_view underline = "\x1b[4m";
    static constexpr std::string_view reset = "\x1b[0m";

    const std::vector<std::string> spinners{ "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
} // end namespace

#endif // !POAC_IO_CLI_HPP
