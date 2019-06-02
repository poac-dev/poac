#ifndef POAC_SUBCMD_ROOT_HPP
#define POAC_SUBCMD_ROOT_HPP

#include <cstdlib>
#include <iostream>
#include <string>

#include <boost/dll/runtime_symbol_info.hpp>

#include "../core/except.hpp"


namespace poac::subcmd {
    struct root {
        static std::string summary() {
            return "Display the root installation directory";
        }
        static std::string options() {
            return "<Nothing>";
        }

        // Reference: https://www.boost.org/doc/libs/1_65_1/doc/html/boost/dll/program_location.html
        template <typename VS>
        int operator()([[maybe_unused]] VS&& vs) {
            namespace fs = boost::filesystem;
            boost::system::error_code ec;
            const auto loc = boost::dll::program_location(ec);
            std::cout << fs::read_symlink(loc, ec).parent_path().string() << std::endl;

            if (ec.failed()) {
                throw core::except::error("Could not get root installation directory");
            }
            return EXIT_SUCCESS;
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_ROOT_HPP
