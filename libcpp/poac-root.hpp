//
// Summary: Display the root directory.
// Options: <Nothing>
//
#ifndef __POAC_ROOT_HPP__
#define __POAC_ROOT_HPP__

#include <iostream>
#include <string>
#include <cstdlib>

namespace poac { namespace root {
    [[noreturn]] void unexpected_err() {
        std::cerr << "Unexpected error" << std::endl;
        std::exit(EXIT_FAILURE);
    }
    std::string get() {
        if (const char* root = std::getenv("POAC_ROOT"); root != nullptr) return root;
        else unexpected_err();
    }
    void run([[maybe_unused]] std::vector<std::string>&& argv) {
        std::cout << get() << std::endl;
    }
}} // end root namespace // end poac namespace

#endif
