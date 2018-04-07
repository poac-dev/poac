//
// Summary: Show the current poac version.
// Options: <Nothing>
//
#ifndef POAC_OPTION_VERSION_HPP
#define POAC_OPTION_VERSION_HPP

#define POAC_VERSION "0.0.5"

#include <iostream>


namespace poac { namespace option { namespace version {
    static const char* summary = "Show the current poac version.";
    static const char* options = "<Nothing>";

    struct t {
        void operator()() { std::cout << POAC_VERSION << std::endl; }
    };
}}} // end namespace
#endif