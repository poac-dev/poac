#ifndef POAC_SUBCMD_LOGIN_HPP
#define POAC_SUBCMD_LOGIN_HPP

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstdlib>

#include <boost/filesystem.hpp>

#include "../core/exception.hpp"
#include "../io/cli.hpp"
#include "../io/file.hpp"


namespace poac::subcmd {
    namespace _login {
        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int _main(VS&& argv) {
            namespace fs     = boost::filesystem;
            namespace exception = core::exception;

            if (fs::create_directories(io::file::path::poac_state_dir)) {
                throw exception::invalid_second_arg("login");
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
                throw exception::invalid_second_arg("login");
            }

            return EXIT_SUCCESS;
        }

        void check_arguments(const std::vector<std::string> &argv) {
            namespace exception = core::exception;
            if (argv.size() != 1) {
                throw exception::invalid_second_arg("login");
            }
        }
    }

    struct login {
        static std::string summary() { return "Login to poac.pm"; }
        static std::string options() { return "<token>"; }
        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int operator()(VS&& argv) {
            _login::check_arguments(argv);
            return _login::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_LOGIN_HPP
