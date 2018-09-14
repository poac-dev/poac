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

    void set_left(const int&& n) { std::cout << std::setw(n) << std::left; }

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

    const std::vector<std::string> spinners{ "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
} // end namespace

#endif // !POAC_IO_CLI_HPP
