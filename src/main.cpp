#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <variant>

#include <poac/poac.hpp>

int handle(std::string_view cmd, std::vector<std::string>&& args) noexcept {
    try {
        const auto error = bin::poac::exec(std::move(cmd), std::move(args));
        if (!error) {
            return EXIT_SUCCESS;
        }
        if (std::holds_alternative<poac::core::except::Error::InvalidSecondArg>(error->state)) {
            handle("help", std::vector<std::string>{error->what()});
        } else {
            std::cerr << poac::io::term::error << error->what() << std::endl;
        }
        return EXIT_FAILURE;
    } catch (const poac::io::config::exception& e) {
        // Remove [error] of top
        std::cerr << poac::io::term::error << std::string(e.what()).substr(8) << std::endl;
        return EXIT_FAILURE;
    } catch (const toml::exception& e) {
        // Remove [error] of top
        std::cerr << poac::io::term::error << std::string(e.what()).substr(8) << std::endl;
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << poac::io::term::error << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int main(int argc, const char** argv) noexcept {
    // TODO:
//    try {
//        const auto args = poac::core::cli::cli.parse(std::vector<std::string>(argv, argv + argc));
//        const auto error = poac::core::cli::exec(args[0], std::move(args[1..]));
//        if (!error) {
//            return EXIT_SUCCESS;
//        }
//        if (std::holds_alternative<poac::core::except::Error::InvalidSecondArg>(error->state)) {
//            handle("help", std::vector<std::string>{error->what()});
//        } else {
//            std::cerr << poac::io::term::error << error->what() << std::endl;
//        }
//        return EXIT_FAILURE;
//    } catch (const std::exception& e) {
//        std::cerr << poac::io::term::error << e.what() << std::endl;
//        return EXIT_FAILURE;
//    } catch (...) {
//        std::cerr << poac::io::term::error << "Unexpected error" << std::endl;
//        return EXIT_FAILURE;
//    }

    using namespace std::string_literals;
    // argv[0]: poac, argv[1]: install, argv[2]: 1, ...

    //$ poac install --help => exec("--help", ["install"])
    if (argc == 3 && ((argv[2] == "-h"s) || (argv[2] == "--help"s))) {
        return handle(argv[2], std::vector<std::string>{argv[1]});
    }
    //$ poac install 1 2 3 => exec("install", ["1", "2", "3"])
    else if (argc >= 3) {
        return handle(argv[1], std::vector<std::string>(argv + 2, argv + argc));
    }
    //$ poac install => exec("install", [])
    else if (argc >= 2) {
        return handle(argv[1], std::vector<std::string>{});
    }
    //$ poac => exec("--help", [])
    else {
        handle("help", std::vector<std::string>{});
        return EXIT_FAILURE;
    }
}
