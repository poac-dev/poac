#ifndef POAC_IO_YAML_HPP
#define POAC_IO_YAML_HPP

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <optional>
#include <vector>
#include <fstream>

#include <boost/predef.h>
#include <boost/filesystem.hpp>
#include <yaml-cpp/yaml.h>

#include <poac/core/except.hpp>

namespace poac::io::yaml {
    namespace detail {
        struct wrapper {
            YAML::Node node;
            explicit wrapper(const YAML::Node& n) {
                // Argument-dependent lookup (ADL)
                node = Clone(n);
            }
            template <typename Key>
            wrapper& operator->*(const Key& key) {
                this->node = node[key];
                return *this;
            }
        };
        template <typename T>
        T dig(const YAML::Node& node) {
            return node.as<T>();
        }
        template <typename T, typename... Keys>
        T dig(const YAML::Node& node, Keys&&... keys) {
            return dig<T>((wrapper(node) ->* ... ->* keys).node);
        }

        // Private member accessor
        using YAML_Node_t = bool YAML::Node::*;
#if BOOST_COMP_MSVC
        template <class T>
        struct accessor {
            static T m_isValid;
            static T get() noexcept {
                return m_isValid;
            }
        };
        template <class T>
        T accessor<T>::m_isValid;

        template <class T, T V>
        struct bastion {
            bastion() { accessor<T>::m_isValid = V; }
        };

        template struct bastion<YAML_Node_t, &YAML::Node::m_isValid>;
        using access = accessor<YAML_Node_t>;
#else
        template <class T, T V>
        struct accessor {
            static constexpr T m_isValid = V;
            static T get() noexcept {
                return m_isValid;
            }
        };
        template <typename T>
        using bastion = accessor<T, &YAML::Node::m_isValid>;
        using access = bastion<YAML_Node_t>;
#endif

        template <typename Head>
        std::optional<const char*>
        read(const YAML::Node& node, Head&& head) {
            if (!(node[head].*access::get())) {
                return head;
            } else {
                return std::nullopt;
            }
        }
        template <typename Head, typename... Tail>
        std::optional<const char*>
        read(const YAML::Node& node, Head&& head, Tail&&... tail) {
            if (!(node[head].*access::get())) {
                return head;
            } else {
                return read(node, std::forward<Tail>(tail)...);
            }
        }

        template <typename T>
        std::optional<T>
        get(const YAML::Node& node) noexcept {
            try {
                return detail::dig<T>(node);
            }
            catch (...) {
                return std::nullopt;
            }
        }
        template <typename T, typename... Args>
        std::optional<T>
        get(const YAML::Node& node, Args&&... args) noexcept {
            try {
                return detail::dig<T>(node, args...);
            }
            catch (...) {
                return std::nullopt;
            }
        }
        template <typename... Args>
        bool get(const YAML::Node& node, Args&&... args) noexcept {
            try {
                return detail::dig<bool>(node, args...);
            }
            catch (...) {
                return false;
            }
        }

        template <typename... Args>
        bool contains(const YAML::Node& node, Args&&... args) noexcept {
            // has_value -> not contains
            try {
                return !detail::read(node, args...).has_value();
            }
            catch (...) {
                return false;
            }
        }
    }

    struct Config {
        struct Build {
            enum class System {
                Poac,
                CMake,
            };
            std::optional<System> system;
            std::optional<bool> bin;
            std::optional<bool> lib;
            std::optional<std::vector<std::string>> compile_args;
            std::optional<std::vector<std::string>> link_args;
        };

        struct Test {
            enum class Framework {
                Boost,
                Google,
            };
            std::optional<Framework> framework;
        };

        std::optional<std::uint64_t> cpp_version;
        std::optional<std::map<std::string, std::string>> dependencies;
        std::optional<std::map<std::string, std::string>> dev_dependencies;
        std::optional<std::map<std::string, std::string>> build_dependencies;
        std::optional<Build> build;
        std::optional<Test> test;
    };

    enum class PackageType {
        HeaderOnlyLib,
        BuildReqLib,
        Application
    };

    std::string
    to_string(PackageType package_type) noexcept {
        switch (package_type) {
            case PackageType::HeaderOnlyLib:
                return "header-only library";
            case PackageType::BuildReqLib:
                return "build-required library";
            case PackageType::Application:
                return "application";
        }
    }

    std::optional<PackageType>
    to_package_type(const std::optional<std::string>& str) noexcept {
        if (!str.has_value()) {
            return std::nullopt;
        } else if (str.value() == "header-only library") {
            return PackageType::HeaderOnlyLib;
        } else if (str.value() == "build-required library") {
            return PackageType::BuildReqLib;
        } else if (str.value() == "application") {
            return PackageType::Application;
        } else {
            return std::nullopt;
        }
    }

    struct Lockfile {
        std::string timestamp;
        struct Package {
            std::string version;
            PackageType package_type;
            std::optional<std::map<std::string, std::string>> dependencies;
        };
        std::map<std::string, Package> dependencies;
    };

    namespace detail {
        std::optional<Config::Build::System>
        to_build_system(const std::optional<std::string>& str) noexcept {
            if (!str.has_value()) {
                // If not specified poac will be selected as default.
                return Config::Build::System::Poac;
            } else if (str.value() == "poac") {
                return Config::Build::System::Poac;
            } else if (str.value() == "cmake") {
                return Config::Build::System::CMake;
            } else {
                return std::nullopt;
            }
        }

        std::optional<Config::Test::Framework>
        to_test_framework(const std::optional<std::string>& str) noexcept {
            if (!str.has_value()) {
                return std::nullopt;
            } else if (str.value() == "boost") {
                return Config::Test::Framework::Boost;
            } else if (str.value() == "google") {
                return Config::Test::Framework::Google;
            } else {
                return std::nullopt;
            }
        }

        inline boost::system::error_code ec{};

        std::optional<Config::Test>
        create_config_test(const YAML::Node& config_yaml) noexcept {
            if (contains(config_yaml, "test")) {
                return Config::Test {
                    to_test_framework(get<std::string>(config_yaml, "test", "framework")),
                };
            } else {
                return std::nullopt;
            }
        }

        std::optional<Config::Build>
        create_config_build(const YAML::Node& config_yaml) noexcept {
            if (contains(config_yaml, "build")) {
                return Config::Build {
                    to_build_system(get<std::string>(config_yaml, "build", "system")),
                    get<decltype(Config::Build::bin)::value_type>(config_yaml, "build", "bin"),
                    get<decltype(Config::Build::lib)::value_type>(config_yaml, "build", "lib"),
                    get<decltype(Config::Build::compile_args)::value_type>(config_yaml, "build", "compile_args"),
                    get<decltype(Config::Build::link_args)::value_type>(config_yaml, "build", "link_args"),
                };
            } else {
                return std::nullopt;
            }
        }

        Config
        create_config(const YAML::Node& config_yaml) noexcept {
            return Config {
                get<decltype(Config::cpp_version)::value_type>(config_yaml, "cpp_version"),
                get<decltype(Config::dependencies)::value_type>(config_yaml, "dependencies"),
                get<decltype(Config::dev_dependencies)::value_type>(config_yaml, "dev_dependencies"),
                get<decltype(Config::build_dependencies)::value_type>(config_yaml, "build_dependencies"),
                create_config_build(config_yaml),
                create_config_test(config_yaml),
            };
        }

        Lockfile::Package
        create_lockfile_package(const YAML::Node& value) {
            return Lockfile::Package {
                get<decltype(Lockfile::Package::version)>(value, "version").value(),
                to_package_type(get<std::string>(value, "package_type")).value(),
                get<decltype(Lockfile::Package::dependencies)::value_type>(value, "dependencies"),
            };
        }

        decltype(Lockfile::dependencies)
        create_lockfile_dependencies(const YAML::Node& lockfile) {
            decltype(Lockfile::dependencies) dependencies;
            const auto lock_dependencies = get<std::map<std::string, YAML::Node>>(lockfile, "dependencies");
            for (const auto& [name, node] : lock_dependencies.value()) {
                dependencies[name] = create_lockfile_package(node);
            }
            return dependencies;
        }

        Lockfile
        create_lockfile(const YAML::Node& lockfile) {
            return Lockfile {
                get<decltype(Lockfile::timestamp)>(lockfile, "timestamp").value(),
                create_lockfile_dependencies(lockfile),
            };
        }

        std::optional<YAML::Node>
        load_yaml(const std::string& filename) noexcept {
            try { return YAML::LoadFile(filename); }
            catch (...) { return std::nullopt; }
        }

        std::optional<std::string>
        validate_config(
                const boost::filesystem::path& base = boost::filesystem::current_path(ec),
                const std::string& config_name = "poac.yml"
        ) noexcept {
            const auto config_path = base / config_name;
            if (boost::filesystem::exists(config_path, ec)) {
                return config_path.string();
            } else {
                return std::nullopt;
            }
        }

        std::optional<YAML::Node>
        load_config(const boost::filesystem::path& base, const std::string& config_name) noexcept {
            if (const auto config = validate_config(base, config_name)) {
                if (const auto node = load_yaml(config.value())) {
                    return node;
                }
            }
            return std::nullopt;
        }
    }

    std::optional<Config>
    load(const boost::filesystem::path& base = boost::filesystem::current_path(detail::ec)
    ) noexcept {
        if (const auto config_yaml = detail::load_config(base, "poac.yml")) {
            return detail::create_config(config_yaml.value());
        }
        return std::nullopt;
    }

    std::optional<Lockfile>
    load_lockfile(const boost::filesystem::path& base = boost::filesystem::current_path(detail::ec)
    ) {
        if (const auto lockfile = detail::load_config(base, "poac.lock")) {
            return detail::create_lockfile(lockfile.value());
        }
        return std::nullopt;
    }

    std::string get_timestamp() {
        if (const auto filename = detail::validate_config()) {
            const std::time_t last_time = boost::filesystem::last_write_time(filename.value(), detail::ec);
            return std::to_string(last_time);
        } else {
            throw core::except::error(
                    core::except::msg::does_not_exist("poac.yml"), "\n",
                    core::except::msg::please_exec("`poac init` or `poac new $PROJNAME`"));
        }
    }
} // end namespace
#endif // !POAC_IO_YAML_HPP
