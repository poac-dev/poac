//
// Summary: Display the root directory.
// Options: <Nothing>
//
#ifndef __POAC_ROOT_HPP__
#define __POAC_ROOT_HPP__

#include <iostream>
#include <string>
#include <cstdlib>


namespace poac { namespace subcmd { namespace root {
    static const char* summary = "Display the root directory.";
    static const char* options = "<Nothing>";

    struct run {
        const std::string operator()() {
            std::cout << POAC_ROOT << std::endl;
            return std::string(POAC_ROOT);
        }
    };
}}} // end root namespace // end subcmd namespace // end poac namespace
#endif
