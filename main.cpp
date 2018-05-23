#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>

#include "include/poac.hpp"


int main([[maybe_unused]] int argc, const char** argv) {
//    if (argc > 1) poac::inference::exec(argv[1]);
    try {
        poac::inference::exec(argv[1]); // std::vector<std::string>(argv+1, argv+argc)
    }
    catch (const std::invalid_argument& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
//    else          poac::option::help(std::vector<std::string>(argv+1, argv+argc));
    return EXIT_SUCCESS;
}