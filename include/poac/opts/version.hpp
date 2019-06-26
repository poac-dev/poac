#ifndef POAC_OPTS_VERSION_HPP
#define POAC_OPTS_VERSION_HPP

#include <iostream>
#include <cstdlib>

#include "../util/termcolor2.hpp"

namespace poac::opts {
    namespace _version {
        constexpr auto summary() {
            return termcolor2::make_string("Show the current poac version");
        }
        constexpr auto options() {
            return termcolor2::make_string("<Nothing>");
        }
    }

    struct version {
        template<typename VS>
        int operator()([[maybe_unused]] VS&& argv) {
            std::cout << POAC_VERSION << std::endl;
            return EXIT_SUCCESS;
        }
    };
} // end namespace
#endif // !POAC_OPTS_VERSION_HPP
