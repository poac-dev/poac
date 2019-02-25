#ifndef POAC_SUBCMD_UPDATE_HPP
#define POAC_SUBCMD_UPDATE_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <optional>
#include <cstdlib>

#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "./install.hpp"
#include "../core/exception.hpp"
#include "../core/naming.hpp"
#include "../core/deper/semver.hpp"
#include "../core/deper/resolver.hpp"
#include "../io/file/path.hpp"
#include "../io/file/yaml.hpp"
#include "../io/cli.hpp"
#include "../io/network.hpp"
#include "../util/types.hpp"


// TODO: --selfを指定することで，poacのupdateを行う -> globalなパッケージに対応した時，どうする？
// TODO: --select | --intractive とすると，インタラクティブに選択してupdateできる．
namespace poac::subcmd {
    namespace _update {
        template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int _main(VS&& argv) {
            namespace fs = boost::filesystem;
            namespace exception = core::exception;
            namespace yaml = io::file::yaml;
            namespace cli = io::cli;
            namespace resolver = core::deper::resolver;
            namespace naming = core::naming;

            const bool yes = util::argparse::use_rm(argv, "-y", "--yes");
            const bool all = util::argparse::use_rm(argv, "-a", "--all");
            [[maybe_unused]] const bool outside = util::argparse::use_rm(argv, "--outside");


            if (!io::file::path::validate_dir("deps")) {
                const auto err = "It is the same as executing install command because nothing is installed.";
                cli::echo(cli::to_warning(err));
                _install::_main(std::move(argv)); // FIXME: これだと現状，allの動作になってしまう．-> install hoge の機能がつけば良い
                return EXIT_FAILURE;
            }

            if (all) {
                // FIXME: install.hppと同じ内容が多い
                const auto node = yaml::load_config("deps");
                const auto deps_node = node.at("deps").as<std::map<std::string, YAML::Node>>();
                const resolver::Deps deps = _install::resolve_packages(deps_node);
                resolver::Resolved resolved_deps = resolver::resolve(deps);
                resolver::Backtracked update_deps;

                for (const auto& [name, dep] : resolved_deps.backtracked) {
                    if (dep.source == "poac") {
                        const auto current_name = naming::to_current(dep.source, name, dep.version);
                        std::string current_version;
                        if (const auto yml = yaml::exists_config(fs::path("deps") / current_name)) {
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

                        if (core::deper::semver::Version(dep.version) != current_version) {
                            update_deps[name] = { {current_version}, {dep.source} };
                        }
                    }
                }

                if (update_deps.empty()) {
                    std::cout << "No changes detected." << std::endl;
                    return EXIT_FAILURE;
                }

                for (const auto& [name, dep] : update_deps) {
                    const auto current_version = resolved_deps.backtracked[name].version;
                    std::cout << name << " (Current: " << current_version << " -> Update: ";
                    if (core::deper::semver::Version(current_version) < dep.version) {
                        std::cout << cli::to_green(dep.version) << ")" << std::endl;
                    }
                    else {
                        std::cout << cli::to_yellow(dep.version) << ")" << std::endl;
                    }
                }

                if (!yes) {
                    cli::echo();
                    std::cout << "Do you approve of this update? [Y/n] ";
                    std::string yes_or_no;
                    std::cin >> yes_or_no;
                    std::transform(yes_or_no.begin(), yes_or_no.end(), yes_or_no.begin(), ::tolower);
                    if (!(yes_or_no == "yes" || yes_or_no == "y")) {
                        std::cout << "canceled." << std::endl;
                        return EXIT_FAILURE;
                    }
                }

                // Delete current version
                for (const auto& [name, dep] : update_deps) {
                    const auto current_name = naming::to_current(dep.source, name, resolved_deps.backtracked[name].version);
                    boost::system::error_code ec;
                    fs::remove_all(fs::path("deps") / current_name, ec);
                }

                // Install new version
                cli::echo();
                _install::fetch_packages(update_deps, false, false);

                cli::echo();
                cli::echo(cli::status_done());

                return EXIT_SUCCESS;
            }
            else { // TODO: Individually update
                std::cout << "Individually update is coming soon" << std::endl;
                return EXIT_FAILURE;
//                if (const auto versions = get_versions_api(argv[0])) {
//                    const auto versions_v = util::types::ptree_to_vector<std::string>(*versions);
//
//                }
            }
        }
    }

    struct update {
        static std::string summary() {
            return "Update package";
        }
        static std::string options() {
            return "[ -y | --yes, -a | --all, --outside ]";
        }
        template <typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        int operator()(VS&& argv) {
            return _update::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_UPDATE_HPP
