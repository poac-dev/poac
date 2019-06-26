#ifndef POAC_CORE_DEPER_LOCK_HPP
#define POAC_CORE_DEPER_LOCK_HPP

#include <iostream>
#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

#include "resolve.hpp"
#include "../../io/yaml.hpp"


namespace poac::core::resolver::lock {
    const std::string filename = "poac.lock";

    std::optional<YAML::Node>
    check_timestamp(std::string_view timestamp) {
        namespace yaml = io::yaml;
        if (const auto lock = yaml::load(filename)) {
            if (const auto lock_timestamp = yaml::get<std::string>(*lock, "timestamp")) {
                if (timestamp == *lock_timestamp) {
                    return *lock;
                }
            }
        }
        return std::nullopt;
    }

    std::optional<std::map<std::string, YAML::Node>>
    load_deps(std::string_view timestamp) {
        namespace yaml = io::yaml;
        if (const auto lock = check_timestamp(timestamp)) {
            if (const auto locked_deps = yaml::get<std::map<std::string, YAML::Node>>(*lock, "dependencies")) {
                return *locked_deps;
            }
        }
        return std::nullopt;
    }

    std::optional<resolve::Activated>
    load_deps_deps(const YAML::Node& node) {
        namespace yaml = io::yaml;
        // dependenciesも読む -> 順番に削除していく必要があるためと，対象でないパッケージが依存していることを防ぐため
        if (const auto deps_deps = yaml::get<std::map<std::string, YAML::Node>>(node, "dependencies")) {
            resolve::Activated deps;
            for (const auto&[name2, next_node2] : *deps_deps) {
                const auto version2 = yaml::get_with_throw<std::string>(next_node2, "version");
                const auto source2 = yaml::get_with_throw<std::string>(next_node2, "source");
                deps.push_back({{name2}, {version2}, {source2}, {}});
            }
            return deps;
        }
        else {
            return std::nullopt;
        }
    }

    resolve::Resolved
    create_resolved_deps(const std::map<std::string, YAML::Node>& locked_deps) {
        namespace yaml = io::yaml;

        resolve::Resolved resolved_deps{};
        for (const auto& [name, next_node] : locked_deps) {
            const auto version = yaml::get_with_throw<std::string>(next_node, "version");
            const auto source = yaml::get_with_throw<std::string>(next_node, "source");

            if (const auto deps_deps = load_deps_deps(next_node)) {
                resolved_deps.activated.push_back({ {name}, {version}, {source}, {*deps_deps} });
            }
            else {
                resolved_deps.activated.push_back({ {name}, {version}, {source}, {} });
            }
            resolved_deps.backtracked[name] = { {version}, {source} };
        }
        return resolved_deps;
    }

    // Ignore timestamp check
    std::optional<resolve::Resolved>
    load_ignore_timestamp() {
        namespace yaml = io::yaml;

        if (const auto lock = yaml::load(filename)) {
            if (const auto locked_deps = yaml::get<std::map<std::string, YAML::Node>>(*lock, "dependencies")) {
                return create_resolved_deps(*locked_deps);
            }
        }
        return std::nullopt;
    }

    std::optional<resolve::Resolved>
    load(const std::string& timestamp=io::yaml::get_timestamp()) {
        namespace yaml = io::yaml;

        if (const auto locked_deps = load_deps(timestamp)) {
            return create_resolved_deps(*locked_deps);
        }
        else {
            return std::nullopt;
        }
    }
} // end namespace
#endif // !POAC_CORE_DEPER_LOCK_HPP
