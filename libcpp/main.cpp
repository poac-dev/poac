#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>

#include "include/poac.hpp"


int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    std::string s{ "root" };
//    if (argc > 1) poac::inference::exec(argv[1]);
    try {
        // TODO: この問題の解決策は．特殊化しかない．execのif分岐で実装？？？
        poac::inference::exec("--help");
        poac::inference::help();
    }
    catch (const std::invalid_argument& e) {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
//    else          poac::option::help(std::vector<std::string>(argv+1, argv+argc));
    return EXIT_SUCCESS;
}
