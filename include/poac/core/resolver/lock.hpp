#ifndef POAC_CORE_RESOLVER_LOCK_HPP
#define POAC_CORE_RESOLVER_LOCK_HPP

#include <iostream>
#include <vector>
#include <string>

#include <yaml-cpp/yaml.h>

#include <poac/core/resolver/resolve.hpp>
#include <poac/io/yaml.hpp>

namespace poac::core::resolver::lock {
    const std::string filename = "poac.lock";

    std::optional<YAML::Node>
    check_timestamp(std::string_view timestamp) {
        namespace yaml = io::yaml;
        if (const auto lock = yaml::detail::load_yaml(filename)) {
            if (const auto lock_timestamp = yaml::get<std::string>(lock.value(), "timestamp")) {
                if (timestamp == lock_timestamp.value()) {
                    return lock.value();
                }
            }
        }
        return std::nullopt;
    }

    std::optional<std::map<std::string, YAML::Node>>
    load_deps(std::string_view timestamp) { // TODO: lockファイルもstruct化する
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
            for (const auto& [name, next_node] : *deps_deps) {
                std::string version;
                if (const auto version_opt = yaml::get<std::string>(next_node, "version")) {
                    version = version_opt.value();
                } else {
                    throw except::error(
                            except::msg::key_does_not_exist("version")); // TODO: poac.ymlではなくpoac.lock
                }

                std::string source;
                if (const auto source_opt = yaml::get<std::string>(next_node, "source")) {
                    source = source_opt.value();
                } else {
                    throw except::error(
                            except::msg::key_does_not_exist("source")); // TODO: poac.ymlではなくpoac.lock
                }
                deps.push_back({{name}, {version}, {source}, {}});
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
            std::string version;
            if (const auto version_opt = yaml::get<std::string>(next_node, "version")) {
                version = version_opt.value();
            } else {
                throw except::error(
                        except::msg::key_does_not_exist("version")); // TODO: poac.ymlではなくpoac.lock
            }

            std::string source;
            if (const auto source_opt = yaml::get<std::string>(next_node, "source")) {
                source = source_opt.value();
            } else {
                throw except::error(
                        except::msg::key_does_not_exist("source")); // TODO: poac.ymlではなくpoac.lock
            }

            if (const auto deps_deps = load_deps_deps(next_node)) {
                resolved_deps.activated.push_back({ {name}, {version}, {source}, {deps_deps.value()} });
            } else {
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

        if (const auto lock = yaml::detail::load_yaml(filename)) {
            if (const auto locked_deps = yaml::get<std::map<std::string, YAML::Node>>(lock.value(), "dependencies")) {
                return create_resolved_deps(*locked_deps);
            }
        }
        return std::nullopt;
    }

    std::optional<resolve::Resolved>
    load(const std::string& timestamp= io::yaml::load_timestamp()) {
        if (const auto locked_deps = load_deps(timestamp)) {
            return create_resolved_deps(*locked_deps);
        } else {
            return std::nullopt;
        }
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_LOCK_HPP
