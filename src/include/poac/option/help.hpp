#ifndef POAC_OPTION_HELP_HPP
#define POAC_OPTION_HELP_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>

#include "../core/except.hpp"
#include "../inference.hpp"
#include "../console.hpp"


// Forward-declaration
namespace poac::inference {
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
        if (vs.size() == 0) exec_help();
        else if(vs.size() == 1) echo_option(vs[0]);
        else throw poac::core::invalid_second_argument("--help"); // show only --help's option
    }
    void echo_option(const std::string& arg) {
        try {
            const auto &tmp = poac::inference::apply("options", arg, std::vector<std::string>());
            std::cout << "Usage: poac " << arg << " " << tmp << std::endl;
        }
        catch (const poac::core::invalid_first_argument& e) {
            throw poac::core::invalid_second_argument("--help");
        }
    }
    void exec_help() {
        std::cout << "Usage: poac <command> [<args>]" << std::endl << std::endl;

        poac::console::color::bold();
        std::cout << "Available subcommands:" << std::endl;
        poac::console::color::reset();
        for (const auto& [name, value] : poac::inference::subcmd_map)
            _help(name, value);

        poac::console::color::bold();
        std::cout << "Available options:" << std::endl;
        poac::console::color::reset();
        for (const auto& [name, value] : poac::inference::option_map)
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
            poac::console::color::blue();
            poac::console::color::bold();
            std::cout << "   " << std::setw(9) << std::left << key << "   ";
            poac::console::color::reset();

            poac::console::color::yellow();
            std::cout << _apply("summary", value, std::vector<std::string>()) << std::endl;
            poac::console::color::reset();
        }
    }
};} // end namespace
#endif // !POAC_OPTION_HELP_HPP