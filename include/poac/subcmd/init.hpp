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


namespace poac::subcmd { struct init {
    static const std::string summary() { return "Create the poac.yml."; }
    static const std::string options() { return "<Nothing>"; }

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        namespace fs     = boost::filesystem;

        check_arguments(argv);
        const std::string filename = check_requirements();
        std::ofstream yml_ofs(filename);

        YAML::Node node = YAML::Load(util::ftemplate::poac_yml);
        node["name"] = basename(fs::current_path());

        yml_ofs << node;
        yml_ofs.close();
        std::cout << current() / filename << " was created." << std::endl;
    }

    std::string check_requirements() {
        namespace except = core::exception;

        if (const auto result = io::file::yaml::exists_setting_file()) {
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
                throw except::error("canceled");
        }
        // default setting file name
        return "poac.yml";
    }
    std::string basename(boost::filesystem::path&& s) {
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

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;
        if (!argv.empty()) throw except::invalid_second_arg("init");
    }
};} // end namespace
#endif // !POAC_SUBCMD_INIT_HPP
