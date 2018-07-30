#ifndef POAC_OPTION_HELP_HPP
#define POAC_OPTION_HELP_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include "../core/except.hpp"
#include "../core/inference.hpp"
#include "../io/cli.hpp"


// Forward-declaration
namespace poac::core::inference {
    enum class op_type_e : int;
    auto _apply(const std::string&& func, const op_type_e& type, const std::vector<std::string>&& arg);
    std::string apply(std::string&& func, const std::string& cmd, std::vector<std::string>&& arg);
    extern const std::unordered_map<std::string, op_type_e> subcmd_map;
    extern const std::unordered_map<std::string, op_type_e> option_map;
}

namespace poac::option { struct help {
    static const std::string summary() { return "Display help for a command."; }
    static const std::string options() { return "<subcommad or option>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main([[maybe_unused]] VS&& vs) {
        namespace except = poac::core::except;

        if (vs.size() == 0) exec_help();
        else if(vs.size() == 1) echo_option(vs[0]);
        else throw except::invalid_second_arg("--help"); // show only --help's option
    }
    void echo_option(const std::string& arg) {
        namespace except = poac::core::except;
        try {
            const auto &tmp = poac::core::inference::apply("options", arg, std::vector<std::string>());
            std::cout << "Usage: poac " << arg << " " << tmp << std::endl;
        }
        catch (const except::invalid_first_arg& e) {
            throw except::invalid_second_arg("--help");
        }
    }
    void exec_help() {
        std::cout << "Usage: poac <command> [<args>]" << std::endl << std::endl;

        std::cout << poac::io::cli::bold
                  << "Available subcommands:"
                  << poac::io::cli::reset
                  << std::endl;
        for (const auto& [name, value] : poac::core::inference::subcmd_map)
            _help(name, value);

        std::cout << poac::io::cli::bold
                  << "Available options:"
                  << poac::io::cli::reset
                  << std::endl;
        for (const auto& [name, value] : poac::core::inference::option_map)
            _help(name, value);

        std::cout << std::endl
                  << "See `poac <command> --help` for information on a specific command.\n"
                     "For full documentation, see: https://github.com/poacpm/poac#readme\n";
    }
    template <typename T, typename U>
    void _help(const T& key, const U& value) {
        // Eliminate -h and -v
        // It assumes two characters because the regular expression is slow.
        if (key.size() != 2) {
            std::cout << poac::io::cli::blue << poac::io::cli::bold
                      << "   " << std::setw(9) << std::left << key << "   "
                      << poac::io::cli::reset;

            std::cout << poac::io::cli::yellow
                      << _apply("summary", value, std::vector<std::string>())
                      << poac::io::cli::reset
                      << std::endl;
        }
    }
};} // end namespace
#endif // !POAC_OPTION_HELP_HPP
