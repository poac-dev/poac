#ifndef POAC_CONSOLE_HPP
#define POAC_CONSOLE_HPP

#include <iostream>
#include <string_view>


namespace poac::console {
    static constexpr std::string_view red = "\x1b[31m";
//    static constexpr std::string_view green = "\x1b[32m";
    static constexpr std::string_view yellow = "\x1b[33m";
    static constexpr std::string_view blue = "\x1b[34m";
    static constexpr std::string_view bold = "\x1b[1m";
    static constexpr std::string_view underline = "\x1b[4m";
    static constexpr std::string_view reset = "\x1b[0m";
} // end namespace

#endif // !POAC_CONSOLE_HPP