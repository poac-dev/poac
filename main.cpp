#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "./include/poac/poac.hpp"


using VS = std::vector<std::string>;

// TODO: このあたりの処理をmain.cppがするべきではない．もう一段階抽象化すべき
int error_handling(std::string&& s)
{
    namespace inference = poac::core::infer;
    namespace io = poac::io;
    using namespace std::string_literals;

    std::cerr << io::cli::to_red("ERROR: ") << s << std::endl << std::endl;
    inference::apply("exec"s, "--help"s, VS());
    return EXIT_FAILURE;
}

int exec(std::string&& str, VS&& vs)
{
    namespace inference = poac::core::infer;
    namespace exception = poac::core::exception;
    namespace cli = poac::io::cli;
    using namespace std::string_literals;

    // TODO: 広い空間でcatchするのは危険．Result typeを使用したい
    try {
        return std::stoi(inference::apply("exec"s, str, std::move(vs)));
    }
    catch (const exception::invalid_first_arg& e) {
        return error_handling(e.what());
    }
    catch (const exception::invalid_second_arg& e) {
        inference::apply("exec"s, "--help"s, VS({e.what()}));
        return EXIT_FAILURE;
    }
    catch (const exception::error& e) {
        std::cerr << cli::to_red("ERROR: ") << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const exception::warn& e) {
        std::cout << cli::to_yellow("WARN: ") << e.what() << std::endl;
        return EXIT_SUCCESS;
    }
    catch (const YAML::BadConversion& e) {
        std::cout << cli::error << "poac.yml " << e.what()
                  << std::endl;
        return EXIT_SUCCESS;
    }
    catch (...) {
        std::cerr << cli::to_error("Unexpected error") << std::endl;
        return EXIT_FAILURE;
    }
}

bool equal_str(const std::string& s1, const std::string& s2,
               const std::string& s3)
{
    return (s1 == s2) || (s1 == s3);
}

int main(int argc, const char** argv)
{
    // argv[0]: poac, argv[1]: install, argv[2]: 1, ...

    //$ poac install --help => exec("--help", ["install"])
    if (argc == 3 && equal_str(argv[2], "-h", "--help")) {
        return exec(argv[2], VS({argv[1]}));
    }
    //$ poac install 1 2 3 => exec("install", ["1", "2", "3"])
    else if (argc >= 3) {
        return exec(argv[1], VS(argv + 2, argv + argc));
    }
    //$ poac install => exec("install", [])
    else if (argc >= 2) {
        return exec(argv[1], VS());
    }
    //$ poac => exec("--help", [])
    else {
        exec("--help", VS());
        return EXIT_FAILURE;
    }
}
