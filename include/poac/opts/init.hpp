#ifndef POAC_OPTS_INIT_HPP
#define POAC_OPTS_INIT_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <optional>

#include <boost/filesystem.hpp>

#include <poac/opts/new.hpp>
#include <poac/io/term.hpp>
#include <poac/io/yaml.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::init {
    constexpr auto summary = termcolor2::make_string("Create the poac.yml");
    constexpr auto options = termcolor2::make_string("<Nothing>");

    [[nodiscard]] std::optional<core::except::Error>
    overwrite(const std::string& config_path) {
        std::cout << termcolor2::bold<> << termcolor2::red<>
                  << config_path << " is already exists." << std::endl
                  << std::endl
                  << "See `poac init --help`" << std::endl
                  << std::endl
                  << "Use `poac install <pkg>` afterwards to install a package and" << std::endl
                  << "save it as a dependency in the poac.yml file." << std::endl
                  << termcolor2::reset<> << std::endl;
        if (const auto error = io::term::yes_or_no("Do you want overwrite?")) {
            return error;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    init() {
        namespace fs = boost::filesystem;

        if (const auto config_path = io::yaml::detail::validate_config()) {
            if (const auto error = overwrite(config_path.value())) {
                return error;
            }
        }
        const std::string project_name = fs::basename(fs::current_path());
        core::name::validate_package_name(project_name);

        const std::string config_path = "poac.yml";
        std::ofstream yml_ofs(config_path);
        yml_ofs << _new::files::poac_yml(project_name, "bin");
        std::cout << config_path << " was created." << std::endl;

        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::yaml::Config>&&, std::vector<std::string>&& args) {
        if (!args.empty()) {
            return core::except::Error::InvalidSecondArg::Init;
        }
        return init::init();
    }
} // end namespace
#endif // !POAC_OPTS_INIT_HPP
