#ifndef POAC_OPTS_VERSION_HPP
#define POAC_OPTS_VERSION_HPP

#include <iostream>
#include <cstdlib>

#include "../util/termcolor2.hpp"

namespace poac::opts::version {
    constexpr auto summary = termcolor2::make_string("Show the current poac version");
    constexpr auto options = termcolor2::make_string("<Nothing>");

    int _main(const std::vector<std::string>&) {
        std::cout << POAC_VERSION << std::endl;
        return EXIT_SUCCESS;
    }
} // end namespace
#endif // !POAC_OPTS_VERSION_HPP
