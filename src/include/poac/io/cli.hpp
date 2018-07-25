#ifndef POAC_IO_CLI_HPP
#define POAC_IO_CLI_HPP

#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <thread>
#include <chrono>
#include <cstdio>
#include <unistd.h>


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
    const std::string_view up(unsigned int y) { return "\x1b["+std::to_string(y)+"A"; }
    const std::string_view down(unsigned int y) { return "\x1b["+std::to_string(y)+"B"; }
    const std::string_view right(unsigned int x) { return "\x1b["+std::to_string(x)+"C"; }
    const std::string_view left(unsigned int x) { return "\x1b["+std::to_string(x)+"D"; }
    const std::string_view location(unsigned int x, unsigned int y) { return "\x1b["+std::to_string(x)+";"+std::to_string(y)+"H"; }

    static constexpr std::string_view red = "\x1b[31m";
    static constexpr std::string_view green = "\x1b[32m";
    static constexpr std::string_view yellow = "\x1b[33m";
    static constexpr std::string_view blue = "\x1b[34m";
    static constexpr std::string_view bold = "\x1b[1m";
    static constexpr std::string_view underline = "\x1b[4m";
    static constexpr std::string_view reset = "\x1b[0m";

    const std::vector<std::string> spinners{ "⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏" };
    void loading() {
        for (int i = 0; i < 5; ++i) {
            for (const std::string& s : spinners) {
                std::cout << s << "\b" << std::flush;
                usleep(100000);
//                std::fflush(stdout);
//                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        std::cout << "" << std::endl;
        
    }
} // end namespace

#endif // !POAC_IO_CLI_HPP
