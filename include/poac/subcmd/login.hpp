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
        namespace fs     = boost::filesystem;
        namespace except = core::except;

        if (vs.size() != 1)
            throw except::invalid_second_arg("login");
        std::regex pattern("\\w{8}-(\\w{4}-){3}\\w{12}");
        if (!std::regex_match(vs[0], pattern))
            throw except::invalid_second_arg("login");

        if (fs::create_directories(io::file::path::poac_state_dir))
            throw except::invalid_second_arg("login");

        const std::string token = io::file::path::poac_token_dir.string();
        if (std::ofstream ofs(token); ofs) {
            ofs << vs[0] << std::endl;
            std::cout << io::cli::bold
                      << "Write to " + token
                      << io::cli::reset
                      << std::endl;
        }
        else { // file open error
            throw except::invalid_second_arg("login");
        }
    }
};} // end namespace
#endif // !POAC_SUBCMD_LOGIN_HPP
