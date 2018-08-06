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
    static constexpr std::string_view clr       = "\x1b[2J";
    // カーソル位置からその行の右端までをクリア
    static constexpr std::string_view clr_right = "\x1b[0K";
    // カーソル位置からその行の左端までをクリア
    static constexpr std::string_view clr_left  = "\x1b[1K";
    // カーソル位置の行をクリア
    static constexpr std::string_view clr_line  = "\x1b[2K";
    // カーソル位置を移動
    const std::string_view up(unsigned int y)    { return "\x1b["+std::to_string(y)+"A"; }
    const std::string_view down(unsigned int y)  { return "\x1b["+std::to_string(y)+"B"; }
    const std::string_view right(unsigned int x) { return "\x1b["+std::to_string(x)+"C"; }
    const std::string_view left(unsigned int x)  { return "\x1b["+std::to_string(x)+"D"; }
    const std::string_view location(unsigned int x, unsigned int y)
        { return "\x1b["+std::to_string(x)+";"+std::to_string(y)+"H"; }

    void set_left(const int&& n)
        { std::cout << std::setw(n) << std::left; }

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
