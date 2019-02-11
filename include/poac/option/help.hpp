#ifndef POAC_OPTION_HELP_HPP
#define POAC_OPTION_HELP_HPP

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>

#include "../core/exception.hpp"
#include "../core/inference.hpp"
#include "../io/cli.hpp"


// Forward-declaration
namespace poac::core::infer {
    enum class op_type_e : int;

    template <typename S, typename OpTypeE, typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    auto _apply(S&& func, const OpTypeE& cmd, VS&& arg);
    template <typename S, typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    std::string apply(S&& func, const S& cmd, VS&& arg);

    extern const std::unordered_map<std::string, op_type_e> subcmd_map;
    extern const std::unordered_map<std::string, op_type_e> option_map;
}

// TODO: help文を，コンパイル時に一つの文字列として変換する．

// TODO: optionではなく，helpコマンドとすれば，順序は，init helpを許されなくなるので明快になる．
// TODO: さらに，versionを，poacの部分に埋め込めば(もう一段階抽象化後)，optionを管理する必要がなくなる．
namespace poac::option {
    namespace _help {
        void echo_option(const std::string& arg) {
            namespace exception = core::exception;
            try {
                std::cout << "Usage: poac " << arg << " "
                          << core::infer::apply(std::string("options"), arg, std::vector<std::string>())
                          << std::endl;
            }
            catch (const exception::invalid_first_arg& e) {
                throw exception::invalid_second_arg("--help");
            }
        }

        template<typename T, typename U>
        void show(const T& key, const U& value) {
            // Eliminate -h and -v
            // It assumes two characters because the regular expression is slow.
            if (key.size() != 2) {
                std::cout << io::cli::blue << io::cli::bold
                          << "   " << std::setw(9) << std::left << key << "   "
                          << io::cli::reset;

                std::cout << io::cli::yellow
                          << _apply(std::string("summary"), value, std::vector<std::string>())
                          << io::cli::reset
                          << std::endl;
            }
        }

        void exec_help() {
            std::cout << "Usage: poac <command> [<args>]" << std::endl << std::endl;

            std::cout << io::cli::bold
                      << "Available subcommands:"
                      << io::cli::reset
                      << std::endl;
            for (const auto&[name, value] : core::infer::subcmd_map)
                show(name, value);

            std::cout << io::cli::bold
                      << "Available options:"
                      << io::cli::reset
                      << std::endl;
            for (const auto&[name, value] : core::infer::option_map)
                show(name, value);

            std::cout << std::endl
                      << "See `poac <command> --help` for information on a specific command.\n"
                         "For full documentation, see: https://github.com/poacpm/poac#readme\n";
        }

        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int _main(VS&& vs) {
            namespace exception = core::exception;
            if (vs.size() == 0) { exec_help(); }
            else if (vs.size() == 1) { echo_option(vs[0]); }
            else { throw exception::invalid_second_arg("--help"); }
            // show only --help's option
            return EXIT_SUCCESS;
        }
    }

    struct help {
        static const std::string summary() {
            return "Display help for a command";
        }
        static const std::string options() {
            return "<subcommad or option>";
        }
        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int operator()(VS&& argv) {
            return _help::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_OPTION_HELP_HPP
