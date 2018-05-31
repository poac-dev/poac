#ifndef POAC_CONSOLE_HPP
#define POAC_CONSOLE_HPP

#include <iostream>


namespace poac::console {
    struct color {
        static void red() {
            std::cout << "\x1b[31m";
        }
        static void green() {
            std::cout << "\x1b[32m";
        }
        static void yellow() {
            std::cout << "\x1b[33m";
        }
        static void blue() {
            std::cout << "\x1b[34m";
        }
        static void bold() {
            std::cout << "\x1b[1m";
        }
        static void underline() {
            std::cout << "\x1b[4m";
        }
        static void reset() {
            std::cout << "\x1b[0m";
        }
    };
} // end namespace

#endif // !POAC_CONSOLE_HPP