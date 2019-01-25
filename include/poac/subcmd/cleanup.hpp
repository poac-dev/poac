#ifndef POAC_SUBCMD_CLEANUP_HPP
#define POAC_SUBCMD_CLEANUP_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "./install.hpp"
#include "./uninstall.hpp"
#include "../core/exception.hpp"
#include "../core/resolver.hpp"
#include "../core/naming.hpp"
#include "../io/file/yaml.hpp"
#include "../io/cli.hpp"


namespace poac::subcmd {
    namespace _cleanup {
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main([[maybe_unused]] VS&& argv) {
            namespace except = core::exception;
            namespace yaml = io::file::yaml;
            namespace resolver = core::resolver;
            namespace naming = core::naming;
            namespace fs = boost::filesystem;
            namespace cli = io::cli;


            // create resolved deps
            const auto timestamp = _install::get_yaml_timestamp();
            resolver::Resolved resolved_deps{};
            if (const auto locked_deps = _install::load_locked_deps(timestamp)) {
                resolved_deps = _uninstall::lock_to_resolved(*locked_deps); // lock.hpp等へ？？
            }
            else { // poac.lock does not exist
                const auto deps_node = yaml::load_config("deps").at("deps");
                const resolver::Deps deps = _install::resolve_packages(deps_node);
                resolved_deps = resolver::resolve(deps);
            }

            std::vector<std::string> package_names;
            for (const auto& [name, dep] : resolved_deps.backtracked) {
                const auto package_name = naming::to_current(dep.source, name, dep.version);
                package_names.push_back(package_name);
            }

            // iterate directory
            const auto first = package_names.begin();
            const auto last = package_names.end();
            for (const auto& e : boost::make_iterator_range( fs::directory_iterator("deps"), {} )) {
                const auto found = std::find(first, last, e.path().filename().string());
                if (found == last) { // not found
                    fs::remove_all(e.path());
                    const auto info_state = "Remove unused package " + e.path().filename().string();
                    cli::echo(cli::to_info(info_state));
                }
            }

            // TODO: cleanup _build directory
            // TODO: auto cleanup in install sub-command
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;
            if (!argv.empty()) {
                throw except::invalid_second_arg("cleanup");
            }
        }
    }

    struct cleanup {
        static const std::string summary() {
            return "Delete unnecessary things";
        }
        static const std::string options() {
            return "<Nothing>";
        }
        template<typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) {
            _cleanup::check_arguments(argv);
            _cleanup::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_CLEANUP_HPP
