#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "include/poac.hpp"


using VS = std::vector<std::string>;

int error_handling(std::string&& s) {
    std::cerr << poac::console::red
              << "Error: " << s
              << poac::console::reset
              << std::endl << std::endl;
    poac::inference::apply("exec", "--help", VS());
    return EXIT_FAILURE;
}

template <typename S, typename VS>
int exec(S&& s, VS&& vs) {
    try {
        poac::inference::apply("exec", s, std::move(vs));
        return EXIT_SUCCESS;
    }
    catch (const poac::core::invalid_first_argument& e) {
        return error_handling(e.what());
    }
    catch (const poac::core::invalid_second_argument& e) {
        poac::inference::apply("exec", "--help", std::move(VS({ e.what() })));
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << poac::console::red
                  << "Error: " << "Unexpected error"
                  << poac::console::reset
                  << std::endl << std::endl;
        return EXIT_FAILURE;
    }
}

bool equal_str(const std::string& s1, const std::string& s2, const std::string& s3) {
    return (s1 == s2) || (s1 == s3);
}

int main(int argc, const char** argv) {
    // argv[0]: poac, argv[1]: install, argv[2]: 1, ...

    //$ poac install --help => exec("--help", ["install"])
    if (argc == 3 && equal_str(argv[2], "-h", "--help")) { return exec(argv[2], VS({ argv[1] })); }
    //$ poac install 1 2 3 => exec("install", ["1", "2", "3"])
    else if (argc >= 3) { return exec(argv[1], VS(argv+2, argv+argc)); }
    //$ poac install => exec("install", [])
    else if (argc >= 2) { return exec(argv[1], VS()); }
    //$ poac => exec("--help", [])
    else { exec("--help", VS()); return EXIT_FAILURE; }
}
