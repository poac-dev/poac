#ifndef POAC_OPTS_INIT_HPP
#define POAC_OPTS_INIT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <optional>

#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include "./new.hpp"
#include "../io/cli.hpp"
#include "../io/yaml.hpp"
#include "../core/except.hpp"
#include "../core/name.hpp"
#include "../util/termcolor2.hpp"

namespace poac::opts::init {
    constexpr auto summary = termcolor2::make_string("Create the poac.yml");
    constexpr auto options = termcolor2::make_string("<Nothing>");

    std::string basename(boost::filesystem::path&& s) {
        namespace fs = boost::filesystem;
        std::string tmp = fs::basename(s);
        core::name::validate_package_name(tmp);
        return tmp;
    }

    std::string check_requirements() {
        namespace except = core::except;

        if (const auto result = io::yaml::exists_config()) {
            std::cerr << termcolor2::bold<> << termcolor2::red<>
                      << "Already " << *result << " exists." << std::endl
                      << std::endl
                      << "See `poac init --help`" << std::endl
                      << std::endl
                      << "Use `poac install <pkg>` afterwards to install a package and" << std::endl
                      << "save it as a dependency in the poac.yml file." << std::endl
                      << std::endl
                      << "Do you want overwrite? (Y/n): "
                      << termcolor2::reset<>;
            std::string ans;
            std::cin >> ans;
            std::transform(ans.cbegin(), ans.cend(), ans.begin(), tolower);
            if (ans == "y" || ans == "yes")
                return *result;
            else
                throw except::error("canceled");
        }
        return "poac.yml";
    }

    std::optional<core::except::Error>
    check_arguments(const std::vector<std::string>& argv) noexcept {
        namespace except = core::except;
        if (!argv.empty()) {
            return except::Error::InvalidSecondArg::Init;
        }
        return std::nullopt;
    }

    std::optional<core::except::Error> _main(const std::vector<std::string>& argv) {
        namespace fs = boost::filesystem;
        using io::path::path_literals::operator""_path;

        if (const auto result = check_arguments(argv)) {
            return result;
        }

        const std::string filename = check_requirements();
        std::ofstream yml_ofs(filename);
        yml_ofs << _new::files::poac_yml(basename(fs::current_path()), "bin");
        std::cout << "."_path / filename << " was created." << std::endl;

        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_INIT_HPP
