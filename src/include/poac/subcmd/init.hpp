#ifndef POAC_SUBCMD_INIT_HPP
#define POAC_SUBCMD_INIT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "../io/cli.hpp"
#include "../core/except.hpp"
#include "../util/ftemplate.hpp"


namespace poac::subcmd { struct init {
    static const std::string summary() { return "Create the poac.yml."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main(VS&& vs) {
        namespace fs     = boost::filesystem;
        namespace except = poac::core::except;
        namespace io     = poac::io;
        namespace util   = poac::util;


        if (!vs.empty()) throw except::invalid_second_arg("init");

        fs::path filename("poac.yml");
        if (yml_exists(filename)) {
            std::cerr << io::cli::red
                      << "canceled"
                      << io::cli::reset
                      << std::endl;
            std::exit(EXIT_FAILURE);
        }

        std::ofstream yml(filename.string());
        std::string basename = poac::subcmd::init::basename(".");

        // TODO: Comment disappears
        YAML::Node node = YAML::Load(util::ftemplate::poac_yml);
        node["name"] = basename;

        yml << node;
        yml.close();
        std::cout << current() / filename << " was created." << std::endl;
    }

    // If poac.yml exists
    int yml_exists(boost::filesystem::path& filename) {
        boost::system::error_code error;
        if (const bool result = boost::filesystem::exists(filename, error); result && !error) {
            std::cerr << poac::io::cli::bold << poac::io::cli::red
                      << "Already poac.yml exists." << std::endl
                      << std::endl
                      << "See `poac init --help`" << std::endl
                      << std::endl
                      << "Use `poac install <pkg>` afterwards to install a package and" << std::endl
                      << "save it as a dependency in the poac.yml file." << std::endl
                      << std::endl
                      << "Do you want overwrite? (y/n): "
                      << poac::io::cli::reset;
            std::string ans;
            std::cin >> ans;
            std::transform(ans.cbegin(), ans.cend(), ans.begin(), tolower);
            if (ans == "y" || ans == "yes")
                return EXIT_SUCCESS;
            else
                return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    std::string basename(std::string&& s) {
        namespace fs = boost::filesystem;
        std::string tmp = fs::basename(fs::absolute(fs::path(s)).parent_path());
        conv_prohibit_char(tmp);
        return tmp;
    }
    // To snake_case
    void conv_prohibit_char(std::string& s) {
        std::transform(s.cbegin(), s.cend(), s.begin(), tolower);
        std::replace(s.begin(), s.end(), '-', '_');
    }
    boost::filesystem::path current() {
        namespace fs = boost::filesystem;
        return fs::absolute(fs::path(".")).parent_path();
    }
};} // end namespace
#endif // !POAC_SUBCMD_INIT_HPP
