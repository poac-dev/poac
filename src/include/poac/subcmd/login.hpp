#ifndef POAC_SUBCMD_LOGIN_HPP
#define POAC_SUBCMD_LOGIN_HPP

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <regex>

#include <boost/filesystem.hpp>

#include "../core/except.hpp"
#include "../io/cli.hpp"


namespace poac::subcmd { struct login {
    static const std::string summary() { return "Login to poac.pm."; }
    static const std::string options() { return "<token>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main(VS&& vs) {
        namespace fs = boost::filesystem;

        if (vs.size() != 1)
            throw poac::core::invalid_second_argument("login");
        std::regex pattern("\\w{8}-(\\w{4}-){3}\\w{12}");
        if (!std::regex_match(vs[0], pattern))
            throw poac::core::invalid_second_argument("login");

        const fs::path root = expand_user("~/.poac");
        if (fs::create_directories(root))
            throw poac::core::invalid_second_argument("login");

        const fs::path token = root / fs::path("token");
        if (std::ofstream ofs(token.string()); ofs) {
            ofs << vs[0] << std::endl;
            std::cout << poac::io::cli::bold
                      << "Write to " + token.string()
                      << poac::io::cli::reset
                      << std::endl;
        }
        else { // file open error
            throw poac::core::invalid_second_argument("login");
        }
    }

    // Inspired by https://stackoverflow.com/questions/4891006/how-to-create-a-folder-in-the-home-directory
    std::string expand_user(std::string path) {
        if (!path.empty() && path[0] == '~') {
            assert(path.size() == 1 or path[1] == '/');  // or other error handling
            const char* home = std::getenv("HOME");
            if (home || ((home = std::getenv("USERPROFILE")))) {
                path.replace(0, 1, home);
            }
            else {
                const char *hdrive = std::getenv("HOMEDRIVE"),
                           *hpath  = std::getenv("HOMEPATH");
                assert(hdrive);  // or other error handling
                assert(hpath);
                path.replace(0, 1, std::string(hdrive) + hpath);
            }
        }
        return path;
    }
};} // end namespace
#endif // !POAC_SUBCMD_LOGIN_HPP
