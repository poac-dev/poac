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
        // Check if the deps directory exists.
        // Create directory and file(ftemplate).
        // echo
        // echo "Your \"$1\" project was created successfully."
        // echo
        // echo
        // echo "Go into your project by running:"
        // echo "    $ cd $1"
        // echo
        // echo "Start your project with:"
        // echo "    $ poac install hello_world"
        // echo "    $ poac run main.cpp"
        // echo
    }
};} // end namespace
#endif