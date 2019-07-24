#ifndef POAC_OPTS_CLEANUP_HPP
#define POAC_OPTS_CLEANUP_HPP

#include <iostream>
#include <string>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <poac/opts/install.hpp>
#include <poac/opts/uninstall.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/core/resolver/resolve.hpp>
#include <poac/io/term.hpp>
#include <poac/io/yaml.hpp>
#include <poac/util/termcolor2.hpp>

namespace poac::opts::cleanup {
    constexpr auto summary = termcolor2::make_string("Delete unnecessary things");
    constexpr auto options = termcolor2::make_string("<Nothing>");

    [[nodiscard]] std::optional<core::except::Error>
    cleanup(std::optional<io::yaml::Config>&& config) {
        // create resolved deps
        core::resolver::resolve::ResolvedDeps resolved_deps{};
//        if (const auto locked_deps = core::resolver::lock::load()) {
//            resolved_deps = locked_deps.value();
//        } else { // poac.lock does not exist
            const auto dependencies = config->dependencies.value(); // yaml::load_config("deps").as<std::map<std::string, YAML::Node>>();
            const core::resolver::resolve::NoDuplicateDeps deps = install::resolve_packages(dependencies);
            resolved_deps = core::resolver::resolve::resolve(deps);
//        }

        std::vector<std::string> package_names;
        for (const auto& dep : resolved_deps.no_duplicate_deps) {
            const auto package_name = core::name::to_current(dep.first);
            package_names.push_back(package_name);
        }

        // iterate directory
        auto first = package_names.cbegin();
        auto last = package_names.cend();
        for (const auto& e : boost::make_iterator_range( boost::filesystem::directory_iterator("deps"), {} )) {
            const auto found = std::find(first, last, e.path().filename().string());
            if (found == last) { // not found
                boost::filesystem::remove_all(e.path());
                const auto info_state = "Remove unused package " + e.path().filename().string();
                std::cout << io::term::info << info_state << std::endl;
            }
        }

        return std::nullopt;
        // TODO: cleanup _build directory
        // TODO: auto cleanup in install sub-command
    }

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::yaml::Config>&& config, std::vector<std::string>&& args) {
        if (!args.empty()) {
            return core::except::Error::InvalidSecondArg::Cleanup;
        }
        return cleanup::cleanup(std::move(config));
    }
} // end namespace
#endif // !POAC_OPTS_CLEANUP_HPP
