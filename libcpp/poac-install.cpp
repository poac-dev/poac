//
// Summary: Install packages.
// Options: [<pkg-name>]
//
#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <string>
#include <string_view>
#include <cstdlib>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;


std::string judge_version(std::string_view tmp) {
    if (const std::size_t pos = tmp.find('@'); pos != std::string::npos)
        return std::string(tmp.data(), pos+1);
    else
        return "newest";
}

void _install(const std::string& pkg_name) {
    const std::string url = "https://re.poac.pm";
    const std::string filename = judge_version(pkg_name) + ".tar.gz";
    std::system(("curl -fsSL "+url+"/"+pkg_name+"/"+filename+" -o ./"+filename).c_str());
}

void exec_install(int argc, const char** s) {
    std::vector<std::string> tmp(s+1, s+argc);
    for (const std::string& n : tmp) _install(n);
}

void call_help(std::string_view argv) {
    using namespace std::literals::string_literals;
    if (const char* root = std::getenv("POAC_ROOT"); root != nullptr) {
        std::system((root + "/libexec/poac---help "s + argv.data()).c_str());
    }
    else {
        std::cerr << "Unexpected error" << std::endl;
        std::exit(EXIT_FAILURE);
    }
}

int main(int argc, const char** argv) {
    if (argc > 1) {
        if (!std::strcmp(argv[1], "--help") || !std::strcmp(argv[1], "-h")) call_help("");
        else exec_install(argc, argv);
    }
    else {
        std::cerr << "poac install: illegal option -- $@" << std::endl;
        call_help("install");
        std::exit(EXIT_FAILURE);
    }
}
