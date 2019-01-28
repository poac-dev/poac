#ifndef POAC_SUBCMD_INIT_HPP
#define POAC_SUBCMD_INIT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "../io/cli.hpp"
#include "../io/file/yaml.hpp"
#include "../core/exception.hpp"
#include "../util/ftemplate.hpp"


namespace poac::subcmd {
    namespace _init {
        // To snake_case
        void conv_prohibit_char(std::string& s) {
            std::transform(s.cbegin(), s.cend(), s.begin(), tolower);
            std::replace(s.begin(), s.end(), '-', '_');
        }

        std::string basename(boost::filesystem::path&& s) {
            namespace fs = boost::filesystem;
            std::string tmp = fs::basename(s);
            conv_prohibit_char(tmp);
            return tmp;
        }

        std::string check_requirements() {
            namespace exception = core::exception;

            if (const auto result = io::file::yaml::exists_config()) {
                std::cerr << io::cli::bold << io::cli::red
                          << "Already " << *result << " exists." << std::endl
                          << std::endl
                          << "See `poac init --help`" << std::endl
                          << std::endl
                          << "Use `poac install <pkg>` afterwards to install a package and" << std::endl
                          << "save it as a dependency in the poac.yml file." << std::endl
                          << std::endl
                          << "Do you want overwrite? (Y/n): "
                          << io::cli::reset;
                std::string ans;
                std::cin >> ans;
                std::transform(ans.cbegin(), ans.cend(), ans.begin(), tolower);
                if (ans == "y" || ans == "yes")
                    return *result;
                else
                    throw exception::error("canceled");
            }
            // default setting file name
            return "poac.yml";
        }

        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main([[maybe_unused]] VS&& argv) {
            namespace fs = boost::filesystem;

            const std::string filename = check_requirements();
            std::ofstream yml_ofs(filename);
            yml_ofs << util::ftemplate::poac_yml(basename(fs::current_path()));
            std::cout << fs::path(".") / filename << " was created." << std::endl;
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace exception = core::exception;
            if (!argv.empty()) throw exception::invalid_second_arg("init");
        }
    }

    struct init {
        static const std::string summary() {
            return "Create the poac.yml";
        }
        static const std::string options() {
            return "<Nothing>";
        }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) {
            _init::check_arguments(argv);
            _init::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_INIT_HPP
