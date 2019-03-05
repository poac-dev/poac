#ifndef POAC_SUBCMD_INIT_HPP
#define POAC_SUBCMD_INIT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "./new.hpp"
#include "../io/cli.hpp"
#include "../io/file/yaml.hpp"
#include "../core/exception.hpp"
#include "../core/naming.hpp"


namespace poac::subcmd {
    namespace _init {
        std::string basename(boost::filesystem::path&& s) {
            namespace fs = boost::filesystem;
            std::string tmp = fs::basename(s);
            core::naming::validate_package_name(tmp);
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
            return "poac.yml";
        }

        template<typename VS>
        int _main([[maybe_unused]] VS&& argv) {
            namespace fs = boost::filesystem;

            const std::string filename = check_requirements();
            std::ofstream yml_ofs(filename);
            yml_ofs << _new::files::poac_yml(basename(fs::current_path()), "bin");
            std::cout << fs::path(".") / filename << " was created." << std::endl;

            return EXIT_SUCCESS;
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace exception = core::exception;
            if (!argv.empty()) throw exception::invalid_second_arg("init");
        }
    }

    struct init {
        static std::string summary() {
            return "Create the poac.yml";
        }
        static std::string options() {
            return "<Nothing>";
        }
        template<typename VS>
        int operator()(VS&& argv) {
            _init::check_arguments(argv);
            return _init::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_INIT_HPP
