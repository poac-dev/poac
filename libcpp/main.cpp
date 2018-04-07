#include <iostream>
#include <string>
#include <vector>

#include "include/poac.hpp"


int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    // 中間インターフェース的な
    std::string s;
    std::cin >> s;
//    if (argc > 1) poac::inference::exec(argv[1]);
//    poac::inference::exec(argv[1]);
    poac::inference::exec(s);
//    else          poac::option::help(std::vector<std::string>(argv+1, argv+argc));
}
