#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>

#include "include/poac.hpp"


using VS = std::vector<std::string>;

int error_handling(std::string&& s) {
    poac::console::color::red();
    std::cerr << "Error: " << s << std::endl << std::endl;
    poac::console::color::reset();
    poac::inference::apply("exec", "--help", VS());
    return EXIT_FAILURE;
}

template <typename S, typename VS>
int exec(S&& s, VS&& vs) {
    try {
        poac::inference::apply("exec", s, std::move(vs));
        return EXIT_SUCCESS;
    }
    catch (const std::invalid_argument& e) {
        return error_handling(e.what());
    }
    catch (...) {
        return error_handling("Unexpected error");
    }
}

int main(int argc, const char** argv) {
    // argv[0]: poac, argv[1]: new, ...
    if (argc >= 3) { return exec(argv[1], VS(argv+2, argv+argc)); }
    else if (argc >= 2) { return exec(argv[1], VS()); }
    else { poac::inference::apply("exec", "--help", VS()); return EXIT_FAILURE; }
}
