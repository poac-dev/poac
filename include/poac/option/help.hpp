//
// Summary: Display help for a command.
// Options: <Nothing>
//
#ifndef POAC_OPTION_HELP_HPP
#define POAC_OPTION_HELP_HPP

#include <iostream>
#include <string>
#include "../inference.hpp"


// Forward-declaration
namespace poac::inference {
    enum class op_type_e : int;
    std::string _apply(const std::string&& func, const op_type_e& type);
    extern const std::unordered_map<std::string, op_type_e> subcmd_map;
    extern const std::unordered_map<std::string, op_type_e> option_map;
}

namespace poac::option { struct help {
    static const std::string summary() { return "Display help for a command."; }
    static const std::string options() { return "<Nothing>"; }

    void operator()() { _main(); }
    void _main() {
        std::cout << "Usage: poac <command> [<args>]" << std::endl << std::endl;
        std::cout << "Available subcommands:" << std::endl;
        for (const auto& [name, value] : poac::inference::subcmd_map)
            _help(name, value);
        std::cout << "Available options:" << std::endl;
        for (const auto& [name, value] : poac::inference::option_map)
            _help(name, value);
        std::cout << std::endl
                  << "See `poac <command> --help` for information on a specific command.\n"
                     "For full documentation, see: https://github.com/poacpm/poac#readme\n";
    }
    template <typename T, typename U>
    void _help(const T& key, const U& value) {
        std::cout << "   " << std::setw(9) << std::left << key
                  << "   " << _apply("summary", value) << std::endl;
    }
};} // end namespace
#endif