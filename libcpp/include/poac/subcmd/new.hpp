//
// Summary: Create a new poacpm project.
// Options: <project-name>
//
#ifndef POAC_SUBCMD_NEW_HPP
#define POAC_SUBCMD_NEW_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>


namespace poac::subcmd { struct new_ {
    static const std::string summary() { return "Create a new poacpm project."; }
    static const std::string options() { return "<project-name>"; }

    void operator()() { _new(); }

    void _new() {

    }
};} // end namespace
#endif