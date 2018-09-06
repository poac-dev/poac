#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

#include "./include/poac/poac.hpp"


using VS = std::vector<std::string>;

// TODO: このあたりの処理をmain.cppがするべきではない．もう一段階抽象化すべき
int error_handling(std::string&& s) {
    namespace inference = poac::core::infer;
    namespace io        = poac::io;

    std::cerr << io::cli::red
              << "Error: " << s
              << io::cli::reset
              << std::endl << std::endl;
    inference::apply(std::string("exec"), std::string("--help"), VS());
    return EXIT_FAILURE;
}

int exec(std::string&& s, VS&& vs) {
    namespace inference = poac::core::infer;
    namespace except    = poac::core::exception;
    namespace io        = poac::io;

    // TODO: 広い空間でcatchするのは危険．Result typeを使用したい
    try {
        inference::apply(std::string("exec"), s, std::move(vs));
        return EXIT_SUCCESS;
    }
    catch (const except::invalid_first_arg& e) {
        return error_handling(e.what());
    }
    catch (const except::invalid_second_arg& e) {
        inference::apply(std::string("exec"), std::string("--help"), VS({ e.what() }));
        return EXIT_FAILURE;
    }
    catch (const except::error& e) {
        std::cerr << io::cli::red << "ERROR: " << io::cli::reset << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const except::warn& e) {
        std::cout << io::cli::yellow << "WARN: " << io::cli::reset << e.what() << std::endl;
        return EXIT_SUCCESS;
    }
    catch (...) {
        std::cerr << io::cli::red
                  << "Error: " << "Unexpected error"
                  << io::cli::reset
                  << std::endl;
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
