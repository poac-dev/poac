#ifndef POAC_IO_CONFIG_HPP
#define POAC_IO_CONFIG_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <vector>

#include <boost/filesystem.hpp>
#include <toml.hpp>

#include <poac/core/except.hpp>
#include <poac/io/path.hpp>

namespace poac::io::config {
    namespace detail {
        inline boost::system::error_code ec{};

        [[noreturn]] inline void
        rethrow_bad_cast(const std::string& what) {
            throw toml::type_error(
                    "[error] value type should be" + what.substr(what.rfind(' ', what.find('\n'))));
        }

        //
        // find and force T type
        // TODO: もし，Tに変換できなければthrowし，そのvalueが存在しない場合はstd::out_of_rangeを消し，std::nulloptを返す
        //
        template <typename T, typename C, template <typename ...> class M, template <typename ...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >>
        find_force_opt(const toml::basic_value<C, M, V>& v, const toml::key& key) {
            try {
                return toml::find<T>(v, key);
            } catch (const toml::type_error& e) {
                rethrow_bad_cast(e.what());
            } catch (...) {
                return std::nullopt;
            }
        }

        template <typename T, typename C, template <typename ...> class M, template <typename ...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >>
        find_force_opt(toml::basic_value<C, M, V>& v, const toml::key& key) {
            try {
                return toml::find<T>(v, key);
            } catch (const toml::type_error& e) {
                rethrow_bad_cast(e.what());
            } catch (...) {
                return std::nullopt;
            }
        }

        template <typename T, typename C, template <typename ...> class M, template <typename ...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >>
        find_force_opt(toml::basic_value<C, M, V>&& v, const toml::key& key) {
            try {
                return toml::find<T>(std::move(v), key);
            } catch (const toml::type_error& e) {
                rethrow_bad_cast(e.what());
            } catch (...) {
                return std::nullopt;
            }
        }

        template <typename T, typename C, template <typename ...> class M,
                template <typename ...> class V, typename ... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >>
        find_force_opt(const toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) {
            return find_force_opt<T>(find_opt(v, key), std::forward<Ts>(keys)...);
        }

        template <typename T, typename C, template <typename ...> class M,
                template <typename ...> class V, typename ... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >>
        find_force_opt(toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) {
            return find_force_opt<T>(find_opt(v, key), std::forward<Ts>(keys)...);
        }

        template <typename T, typename C, template <typename ...> class M,
                template <typename ...> class V, typename ... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >>
        find_force_opt(toml::basic_value<C, M, V>&& v, const toml::key& key, Ts&&... keys) {
            return find_force_opt<T>(find_opt(std::move(v), key), std::forward<Ts>(keys)...);
        }

        //
        // find as optional
        // TODO: Tに変換できない場合も，valueが存在しない場合も，同様にstd::nulloptを返却する．
        //
        template <typename T, typename C, template <typename ...> class M, template <typename ...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >>
        find_opt(const toml::basic_value<C, M, V>& v, const toml::key& key) noexcept {
            try {
                return find_force_opt<T>(v, key);
            } catch (...) {
                return std::nullopt;
            }
        }

        template <typename T, typename C, template <typename ...> class M, template <typename ...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >>
        find_opt(toml::basic_value<C, M, V>& v, const toml::key& key) noexcept {
            try {
                return find_force_opt<T>(v, key);
            } catch (...) {
                return std::nullopt;
            }
        }

        template <typename T, typename C, template <typename ...> class M, template <typename ...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >>
        find_opt(toml::basic_value<C, M, V>&& v, const toml::key& key) noexcept {
            try {
                return find_force_opt<T>(std::move(v), key);
            } catch (...) {
                return std::nullopt;
            }
        }

        template <typename T, typename C, template <typename ...> class M,
                  template <typename ...> class V, typename ... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >>
        find_opt(const toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) noexcept {
            return find_opt<T>(find_opt(v, key), std::forward<Ts>(keys)...);
        }

        template <typename T, typename C, template <typename ...> class M,
                  template <typename ...> class V, typename ... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >>
        find_opt(toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) noexcept {
            return find_opt<T>(find_opt(v, key), std::forward<Ts>(keys)...);
        }

        template <typename T, typename C, template <typename ...> class M,
                  template <typename ...> class V, typename ... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >>
        find_opt(toml::basic_value<C, M, V>&& v, const toml::key& key, Ts&&... keys) noexcept {
            return find_opt<T>(find_opt(std::move(v), key), std::forward<Ts>(keys)...);
        }

        std::optional<std::string>
        validate_config(const boost::filesystem::path& base = path::current) noexcept {
            const auto config_path = base / "poac.toml";
            if (boost::filesystem::exists(config_path, ec)) {
                return config_path.string();
            } else {
                return std::nullopt;
            }
        }
    }

    struct Config {
        struct Build {
            enum class System {
                Poac,
                CMake,
            };
            struct Bin {
                std::optional<std::string> path;
                std::optional<std::string> name;
                std::optional<std::string> link;

                template <typename C, template <typename ...> class M, template <typename ...> class V>
                void from_toml(const toml::basic_value<C, M, V>& v) noexcept;
                toml::table into_toml() const;
            };
            struct Properties {
                std::optional<std::vector<std::string>> definitions;
                std::optional<std::vector<std::string>> options;
                std::optional<std::vector<std::string>> libraries;

                template <typename C, template <typename ...> class M, template <typename ...> class V>
                void from_toml(const toml::basic_value<C, M, V>& v) noexcept;
                toml::table into_toml() const;
            };

            std::optional<System> system;
            std::optional<std::vector<Bin>> bins;
            std::optional<Properties> properties;

            template <typename C, template <typename ...> class M, template <typename ...> class V>
            void from_toml(const toml::basic_value<C, M, V>& v) noexcept;
            toml::table into_toml() const;
        };

        struct Test {
            enum class Framework {
                Boost,
                Google,
            };
            std::optional<Framework> framework;

            template <typename C, template <typename ...> class M, template <typename ...> class V>
            void from_toml(const toml::basic_value<C, M, V>& v) noexcept;
            toml::table into_toml() const;
        };

        std::optional<std::uint16_t> cpp;
        std::optional<std::unordered_map<std::string, std::string>> dependencies;
        std::optional<std::unordered_map<std::string, std::string>> dev_dependencies;
        std::optional<std::unordered_map<std::string, std::string>> build_dependencies;
        std::optional<Build> build;
        std::optional<Test> test;
//        std::optional<std::unordered_map<std::string, toml::value>> target;

        template <typename C, template <typename ...> class M, template <typename ...> class V>
        void from_toml(const toml::basic_value<C, M, V>& v);
        toml::table into_toml() const;
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

        std::string
        to_string(Config::Build::System system) noexcept {
            switch (system) {
                case Config::Build::System::Poac:
                    return "poac";
                case Config::Build::System::CMake:
                    return "cmake";
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

        std::string
        to_string(Config::Test::Framework framework) noexcept {
            switch (framework) {
                case Config::Test::Framework::Boost:
                    return "boost";
                case Config::Test::Framework::Google:
                    return "google";
            }
        }
    }

    template <typename C, template <typename ...> class M, template <typename ...> class V>
    void Config::from_toml(const toml::basic_value<C, M, V>& v) {
        cpp = detail::find_force_opt<std::uint16_t>(v, "cpp"); // TODO: package
        std::cout << std::boolalpha << cpp.has_value() << std::endl;

        // TODO: ここで，cfgのパースをする..?? -> tomlとして，exceptionを出したい．

        for (const auto& [key, value] : toml::find<toml::table>(v, "target")) {
            std::cout << key << std::endl;
            if (key == "cfg(os = \"macos\")") {
                for (const auto& [key2, value2] : toml::find<toml::table>(value, "profile")) {
                    std::cout << key2 << std::endl;
                }
            }
            // TODO: keyを一個ずつ，parseしていく！！！ -> もし，存在しないものとか，文法エラーは，toml::format_errorとしてthrow
        }

        dependencies = detail::find_force_opt<std::unordered_map<std::string, std::string>>(v, "dependencies");
        dev_dependencies = detail::find_force_opt<std::unordered_map<std::string, std::string>>(v, "dev-dependencies");
        build_dependencies = detail::find_force_opt<std::unordered_map<std::string, std::string>>(v, "build-dependencies");
        build = detail::find_force_opt<Build>(v, "build");
        test = detail::find_force_opt<Test>(v, "test");
    }
    toml::table Config::into_toml() const {
        toml::table t{};
        if (cpp.has_value()) {
            t.emplace("cpp", cpp.value());
        }
        if (dependencies.has_value()) {
            t.emplace("dependencies", dependencies.value());
        }
        if (dev_dependencies.has_value()) {
            t.emplace("dev-dependencies", dev_dependencies.value());
        }
        if (build_dependencies.has_value()) {
            t.emplace("build-dependencies", build_dependencies.value());
        }
        if (build.has_value()) {
            t.emplace("build", build.value());
        }
        if (test.has_value()) {
            t.emplace("test", test.value());
        }
        return t;
    }

    template <typename C, template <typename ...> class M, template <typename ...> class V>
    void Config::Build::from_toml(const toml::basic_value<C, M, V>& v) noexcept {
        system = detail::to_build_system(detail::find_opt<std::string>(v, "system"));
        bins = detail::find_opt<std::vector<Bin>>(v, "bin");
        properties = detail::find_opt<Properties>(v, "properties");
    }
    toml::table Config::Build::into_toml() const {
        toml::table t{};
        if (system.has_value()) {
            t.emplace("system", detail::to_string(system.value()));
        }
        if (bins.has_value()) {
            t.emplace("bin", bins.value());
        }
        if (properties.has_value()) {
            t.emplace("properties", properties.value());
        }
        return t;
    }

    template <typename C, template <typename ...> class M, template <typename ...> class V>
    void Config::Build::Bin::from_toml(const toml::basic_value<C, M, V>& v) noexcept {
        path = detail::find_opt<std::string>(v, "path");
        name = detail::find_opt<std::string>(v, "name");
        link = detail::find_opt<std::string>(v, "link");
    }
    toml::table Config::Build::Bin::into_toml() const {
        toml::table t{};
        if (path.has_value()) {
            t.emplace("path", path.value());
        }
        if (name.has_value()) {
            t.emplace("name", name.value());
        }
        if (link.has_value()) {
            t.emplace("link", link.value());
        }
        return t;
    }

    template <typename C, template <typename ...> class M, template <typename ...> class V>
    void Config::Build::Properties::from_toml(const toml::basic_value<C, M, V>& v) noexcept {
        definitions = detail::find_opt<std::vector<std::string>>(v, "definitions");
        options = detail::find_opt<std::vector<std::string>>(v, "options");
        libraries = detail::find_opt<std::vector<std::string>>(v, "libraries");
    }
    toml::table Config::Build::Properties::into_toml() const {
        toml::table t{};
        if (definitions.has_value()) {
            t.emplace("definitions", definitions.value());
        }
        if (options.has_value()) {
            t.emplace("options", options.value());
        }
        if (libraries.has_value()) {
            t.emplace("libraries", libraries.value());
        }
        return t;
    }

    template <typename C, template <typename ...> class M, template <typename ...> class V>
    void Config::Test::from_toml(const toml::basic_value<C, M, V>& v) noexcept {
        framework = detail::to_test_framework(detail::find_opt<std::string>(v, "framework"));
    }
    toml::table Config::Test::into_toml() const {
        toml::table t{};
        if (framework.has_value()) {
            t.emplace("framework", detail::to_string(framework.value()));
        }
        return t;
    }

    template <typename C>
    std::optional<C>
    load_toml(const boost::filesystem::path& base, const std::string& fname) {
        if (boost::filesystem::exists(base / fname, detail::ec)) {
            const auto config_toml = toml::parse<toml::preserve_comments>(fname);
            const auto config = toml::get<C>(config_toml);
            return config;
        } else {
            return std::nullopt;
        }
    }

    std::optional<Config>
    load(const boost::filesystem::path& base = path::current) {
         return load_toml<Config>(base, "poac.toml");
    }

    std::string
    get_timestamp() {
        if (const auto filename = config::detail::validate_config()) {
            return std::to_string(boost::filesystem::last_write_time(filename.value(), config::detail::ec));
        } else {
            throw core::except::error(
                    core::except::msg::does_not_exist("poac.toml"), "\n",
                    core::except::msg::please_exec("`poac init` or `poac new $PROJNAME`"));
        }
    }
} // end namespace
#endif // !POAC_IO_CONFIG_HPP
