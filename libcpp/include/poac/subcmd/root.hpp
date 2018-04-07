//
// Summary: Display the root directory.
// Options: <Nothing>
//
#ifndef POAC_SUBCMD_ROOT_HPP
#define POAC_SUBCMD_ROOT_HPP

#include <iostream>
#include <string>
#include <cstdlib>


namespace poac { namespace subcmd { namespace root {
    static const char* summary = "Display the root directory.";
    static const char* options = "<Nothing>";

    struct t {
        void operator()() { std::cout << POAC_ROOT << std::endl; }
    };
}}} // end namespace
#endif