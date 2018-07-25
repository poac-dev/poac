#ifndef POAC_SUBCMD_LOGIN_HPP
#define POAC_SUBCMD_LOGIN_HPP

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <regex>

#include <boost/filesystem.hpp>

#include "../core/except.hpp"
#include "../io/cli.hpp"
#include "../io/file.hpp"


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

        const fs::path root = poac::io::file::expand_user("~/.poac");
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
};} // end namespace
#endif // !POAC_SUBCMD_LOGIN_HPP
