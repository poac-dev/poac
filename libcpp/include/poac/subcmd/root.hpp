//
// Summary: Display the root directory.
// Options: <Nothing>
//
#ifndef POAC_SUBCMD_ROOT_HPP
#define POAC_SUBCMD_ROOT_HPP

#include <iostream>
#include <string>
#include <cstdlib>


namespace poac { namespace subcmd { struct root {
    static const std::string summary() { return "Display the root directory."; }
    static const std::string options() { return "<Nothing>"; }

    void operator()() { std::cout << POAC_ROOT << std::endl; }
};}} // end namespace
#endif