//
// Summary: Show the current poac version.
// Options: <Nothing>
//
#ifndef POAC_OPTION_VERSION_HPP
#define POAC_OPTION_VERSION_HPP

#define POAC_VERSION "0.0.5"

#include <iostream>


namespace poac::option { struct version {
    static const std::string summary() { return "Show the current poac version."; }
    static const std::string options() { return "<Nothing>"; }

    void operator()() { std::cout << POAC_VERSION << std::endl; }
};} // end namespace
#endif