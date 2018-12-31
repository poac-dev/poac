#ifndef POAC_SUBCMD_LOGIN_HPP
#define POAC_SUBCMD_LOGIN_HPP

#include <iostream>
#include <cstdlib>
#include <fstream>

#include <boost/filesystem.hpp>

#include "../core/exception.hpp"
#include "../io/cli.hpp"
#include "../io/file.hpp"


namespace poac::subcmd {
    namespace _login {
        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            namespace fs     = boost::filesystem;
            namespace except = core::exception;

            if (fs::create_directories(io::file::path::poac_state_dir)) {
                throw except::invalid_second_arg("login");
            }

            const std::string token_path = io::file::path::poac_token_dir.string();
            if (std::ofstream ofs(token_path); ofs) {
                ofs << argv[0] << std::endl;
                std::cout << io::cli::bold
                          << "Write to " + token_path
                          << io::cli::reset
                          << std::endl;
            }
            else { // file open error
                throw except::invalid_second_arg("login");
            }
        }

        void check_arguments(const std::vector<std::string> &argv) {
            namespace except = core::exception;
            if (argv.size() != 1) {
                throw except::invalid_second_arg("login");
            }
        }
    }

    struct login {
        static const std::string summary() { return "Login to poac.pm"; }
        static const std::string options() { return "<token>"; }
        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) {
            _login::check_arguments(argv);
            _login::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_LOGIN_HPP
