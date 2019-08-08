#ifndef POAC_OPTS_INIT_HPP
#define POAC_OPTS_INIT_HPP

#include <future>
#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <optional>

#include <boost/filesystem.hpp>

#include <poac/opts/new.hpp>
#include <poac/io/term.hpp>
#include <poac/io/config.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/util/termcolor2/termcolor2.hpp>

namespace poac::opts::init {
    const clap::subcommand cli =
            clap::subcommand("init")
                .about("Create a new poac package in an existing directory")
                .arg(clap::opt("bin", "Use a binary (application) template [default]"))
                .arg(clap::opt("lib", "Use a library template"))
            ;

    struct Options {
        bool lib;
        bool bin;
    };

    [[nodiscard]] std::optional<core::except::Error>
    overwrite(std::string_view config_path) {
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
    validate() {
        namespace fs = boost::filesystem;
        if (const auto config_path = io::config::detail::validate_config()) {
            if (const auto error = overwrite(config_path.value())) {
                return error;
            }
        }
        if (const auto error = core::name::validate_package_name(fs::basename(fs::current_path()))) {
            return error;
        }
        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    init(init::Options&& opts) {
        namespace fs = boost::filesystem;
        using termcolor2::color_literals::operator""_green;

        if (const auto error = validate()) {
            return error;
        }

        std::cout << "Created: "_green;
        std::ofstream ofs_config("poac.toml");
        if (opts.bin) {
            ofs_config << _new::files::bin::poac_toml(fs::basename(fs::current_path()));
            std::cout << "application ";
        } else {
            ofs_config << _new::files::lib::poac_toml;
            std::cout << "library ";
        }
        std::cout << "package" << std::endl;

        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::future<std::optional<io::config::Config>>&&, std::vector<std::string>&& args) {
        if (args.size() > 1) {
            return core::except::Error::InvalidSecondArg::Init;
        }

        init::Options opts{};
        opts.lib = util::argparse::use(args, "-l", "--lib");
        opts.bin = !opts.lib || util::argparse::use(args, "-b", "--bin");
        return init::init(std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_INIT_HPP
