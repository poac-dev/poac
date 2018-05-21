//
// Summary: Display help for a command.
// Options: <Nothing>
//
#ifndef POAC_OPTION_HELP_HPP
#define POAC_OPTION_HELP_HPP

#include <iostream>
#include <string>


namespace poac::option { struct help {
    static const std::string summary() { return "Display help for a command."; }
    static const std::string options() { return "<Nothing>"; }

    void operator()() {
//        poac::inference::print_summary_all();
        ; // Nothing to do because define `help function` in inference.hpp.
    }
};} // end namespace
#endif