#include <iostream>
#include <string>
#include <vector>
#include "include/poac.hpp" // TODO: ???? なにこれおかしすぎる

int main([[maybe_unused]] int argc, [[maybe_unused]] const char** argv) {
    // 中間インターフェース的な
//    if (argc > 1) poac::exec<std::string(argv[1])>();
//    else          poac::option::help(std::vector<std::string>(argv+1, argv+argc));
    std::cout << poac::subcmd::root::summary << std::endl;
    poac::subcmd::root::run test;
    std::string hoge = test();
    std::cout << hoge << std::endl;;
}
