#ifndef POAC_OPTS_UPDATE_HPP
#define POAC_OPTS_UPDATE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <optional>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <poac/opts/install.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/core/resolver/resolve.hpp>
#include <poac/io/path.hpp>
#include <poac/io/yaml.hpp>
#include <poac/io/term.hpp>
#include <poac/io/net.hpp>
#include <poac/util/semver.hpp>
#include <poac/util/types.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::update {
    constexpr auto summary = termcolor2::make_string("Update a package");
    constexpr auto options = termcolor2::make_string("[ -y | --yes, -a | --all, --outside ]");

    struct Options {
        bool yes;
        bool all;
        bool outside;
    };

    [[nodiscard]] std::optional<core::except::Error>
    individually_update() {
        // TODO: Individually update
        return core::except::Error::General{
                "Individually update is coming soon."
        };
//        if (const auto versions = get_versions_api(argv[0])) {
//            const auto versions_v = util::types::ptree_to_vector<std::string>(*versions);
//        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    all_update(std::optional<io::yaml::Config>&& config, update::Options&& opts) {
        namespace fs = boost::filesystem;
        namespace resolve = core::resolver::resolve;
        using io::path::path_literals::operator""_path;

        const auto deps = install::resolve_packages(config->dependencies.value()); // yaml::load_config("deps").as<std::map<std::string, YAML::Node>>();
        resolve::ResolvedDeps resolved_deps = resolve::resolve(deps);
        resolve::NoDuplicateDeps update_deps;

        for (const auto& [name, version] : resolved_deps.no_duplicate_deps) {
            const std::string current_name = core::name::to_current(name);
            std::string current_version;
            if (const auto yml = io::yaml::detail::validate_config("deps"_path / current_name)) {
                if (const auto deps_config = io::yaml::load(*yml)) {
//                        if (const auto version = deps_config->version) { // TODO: versionは存在しない
//                            current_version = *version;
//                        }
//                        else { // Key not founded TODO: error
//                            current_version = "null";
//                        }
                }
                else { // Could not read
                    current_version = "null";
                }
            }
            else { // Not installed
                current_version = "null";
            }

            if (semver::Version(version) != current_version) {
                update_deps[name] = current_version;
            }
        }

        if (update_deps.empty()) {
            std::cout << "No changes detected." << std::endl;
            return std::nullopt;
        }

        for (const auto& [name, version] : update_deps) {
            const auto current_version = resolved_deps.no_duplicate_deps[name];
            std::cout << name << " (Current: " << current_version << " -> Update: ";
            if (semver::Version(current_version) < version) {
                std::cout << termcolor2::green<> << version << termcolor2::reset<> << ")" << std::endl;
            }
            else {
                std::cout << termcolor2::yellow<> << version << termcolor2::reset<> << ")" << std::endl;
            }
        }

        if (!opts.yes) {
            if (const auto error = io::term::yes_or_no("Do you approve of this update?")) {
                return error;
            }
        }

        // Delete current version
        for (const auto& [name, dep] : update_deps) {
            const std::string current_name = core::name::to_current(name);
            boost::system::error_code error;
            fs::remove_all("deps"_path / current_name, error);
        }

        // Install new version
        std::cout << std::endl;
        install::fetch(update_deps, install::Options{false, false, {}});

        std::cout << std::endl;
        io::term::status_done();

        return std::nullopt;
    }

    [[nodiscard]] std::optional<core::except::Error>
    update(std::optional<io::yaml::Config>&& config, update::Options&& opts) {
        if (!io::path::validate_dir("deps")) {
            return core::except::Error::General{
                "Could not find deps directory.\n"
                "Please execute install command before executing update command."
            };
        } else if (opts.all) {
            return all_update(std::move(config), std::move(opts));
        } else {
            return individually_update();
        }
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::yaml::Config>&& config, std::vector<std::string>&& args) {
        update::Options opts{};
        opts.yes = util::argparse::use(args, "-y", "--yes");
        opts.all = util::argparse::use(args, "-a", "--all");
        opts.outside = util::argparse::use(args, "--outside");
        return update::update(std::move(config), std::move(opts));
    }
} // end namespace
#endif // !POAC_OPTS_UPDATE_HPP
