#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <utility>
#include <functional>

#include "poac-install.hpp"
#include "poac-root.hpp"
#include "poac---help.hpp"


int main(int argc, const char** argv) {
    if (argc > 1) {
        std::map<std::string, std::function<void()>> funcs;
        std::vector<std::string> arg(argv+1, argv+argc);
        funcs["root"] = [&](){ poac::root::run(std::move(arg)); };
        funcs["install"] = [&](){ poac::install::run(std::move(arg)); };
        funcs["--help"] = [&](){ poac::help::run(std::move(arg)); };
        funcs["-h"] = [&](){ poac::help::run(std::move(arg)); };
        if (funcs.find(argv[1]) != funcs.end()) funcs[argv[1]]();
        else poac::help::run();
    }
    else {
        poac::help::run();
    }
}
