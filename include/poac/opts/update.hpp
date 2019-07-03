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
#include <poac/io/cli.hpp>
#include <poac/io/net.hpp>
#include <poac/util/semver.hpp>
#include <poac/util/types.hpp>
#include <poac/util/termcolor2.hpp>

// TODO: --selfを指定することで，poacのupdateを行う -> globalなパッケージに対応した時，どうする？
// TODO: --select | --intractive とすると，インタラクティブに選択してupdateできる．
namespace poac::opts::update {
    constexpr auto summary = termcolor2::make_string("Update a package");
    constexpr auto options = termcolor2::make_string("[ -y | --yes, -a | --all, --outside ]");

    std::optional<core::except::Error>
    _main(const std::vector<std::string>& argv) {
        namespace fs = boost::filesystem;
        namespace except = core::except;
        namespace yaml = io::yaml;
        namespace cli = io::cli;
        namespace resolver = core::resolver::resolve;
        namespace name = core::name;
        using io::path::path_literals::operator""_path;

        std::vector<std::string> argv_cpy = argv;
        const bool yes = util::argparse::use_rm(argv_cpy, "-y", "--yes");
        const bool all = util::argparse::use_rm(argv_cpy, "-a", "--all");
        [[maybe_unused]] const bool outside = util::argparse::use_rm(argv_cpy, "--outside");


        if (!io::path::validate_dir("deps")) {
            const auto err = "It is the same as executing install command because nothing is installed.";
            std::cout << cli::warning << err << std::endl;
            install::_main(std::move(argv_cpy)); // FIXME: これだと現状，allの動作になってしまう．-> install hoge の機能がつけば良い
            return except::Error::General{"Could not find deps directory"};
        }

        if (all) {
            // FIXME: install.hppと同じ内容が多い
            const auto deps_node = yaml::load_config("deps").as<std::map<std::string, YAML::Node>>();
            const resolver::Deps deps = install::resolve_packages(deps_node);
            resolver::Resolved resolved_deps = resolver::resolve(deps);
            resolver::Backtracked update_deps;

            for (const auto& [name, dep] : resolved_deps.backtracked) {
                if (dep.source == "poac") {
                    const auto current_name = name::to_current(dep.source, name, dep.version);
                    std::string current_version;
                    if (const auto yml = yaml::exists_config("deps"_path / current_name)) {
                        if (const auto op_node = yaml::load(*yml)) {
                            if (const auto version = yaml::get<std::string>(*op_node, "version")) {
                                current_version = *version;
                            }
                            else { // Key not founded
                                current_version = "null";
                            }
                        }
                        else { // Could not read
                            current_version = "null";
                        }
                    }
                    else { // Not installed
                        current_version = "null";
                    }

                    if (semver::Version(dep.version) != current_version) {
                        update_deps[name] = { {current_version}, {dep.source} };
                    }
                }
            }

            if (update_deps.empty()) {
                std::cout << "No changes detected." << std::endl;
                return std::nullopt;
            }

            for (const auto& [name, dep] : update_deps) {
                const auto current_version = resolved_deps.backtracked[name].version;
                std::cout << name << " (Current: " << current_version << " -> Update: ";
                if (semver::Version(current_version) < dep.version) {
                    std::cout << termcolor2::green<> << dep.version << termcolor2::reset<> << ")" << std::endl;
                }
                else {
                    std::cout << termcolor2::yellow<> << dep.version << termcolor2::reset<> << ")" << std::endl;
                }
            }

            if (!yes) {
                std::cout << std::endl;
                std::cout << "Do you approve of this update? [Y/n] ";
                std::string yes_or_no;
                std::cin >> yes_or_no;
                std::transform(yes_or_no.begin(), yes_or_no.end(), yes_or_no.begin(), ::tolower);
                if (!(yes_or_no == "yes" || yes_or_no == "y")) {
                    std::cout << "canceled." << std::endl;
                    return std::nullopt;
                }
            }

            // Delete current version
            for (const auto& [name, dep] : update_deps) {
                const auto current_name = name::to_current(dep.source, name, resolved_deps.backtracked[name].version);
                boost::system::error_code error;
                fs::remove_all("deps"_path / current_name, error);
            }

            // Install new version
            std::cout << std::endl;
            install::fetch_packages(update_deps, false, false);

            std::cout << std::endl;
            cli::status_done();

            return std::nullopt;
        }
        else { // TODO: Individually update
            return except::Error::General{
                    "Individually update is coming soon"
            };
//            if (const auto versions = get_versions_api(argv[0])) {
//                const auto versions_v = util::types::ptree_to_vector<std::string>(*versions);
//
//            }
        }
    }
} // end namespace
#endif // !POAC_OPTS_UPDATE_HPP
