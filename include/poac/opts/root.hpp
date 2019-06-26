#ifndef POAC_OPTS_ROOT_HPP
#define POAC_OPTS_ROOT_HPP

#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/dll/runtime_symbol_info.hpp>

#include "../core/except.hpp"
#include "../util/termcolor2.hpp"

namespace poac::opts::root {
    constexpr auto summary() {
        return termcolor2::make_string("Display the root installation directory");
    }
    constexpr auto options() {
        return termcolor2::make_string("<Nothing>");
    }

    // Reference: https://www.boost.org/doc/libs/1_65_1/doc/html/boost/dll/program_location.html
    int _main(const std::vector<std::string>&) {
        namespace fs = boost::filesystem;
        boost::system::error_code error;
        const auto loc = boost::dll::program_location(error);
        if (error) {
            throw core::except::error("Could not get root installation directory");
        }

        const auto ln = fs::read_symlink(loc, error);
        if (!error) {
            std::cout << ln.parent_path().string() << std::endl;
        }
        else {
            std::cout << loc.parent_path().string() << std::endl;
        }
        return EXIT_SUCCESS;
    }
} // end namespace
#endif // !POAC_OPTS_ROOT_HPP
