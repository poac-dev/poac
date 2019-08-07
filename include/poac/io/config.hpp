#ifndef POAC_IO_CONFIG_HPP
#define POAC_IO_CONFIG_HPP

#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <vector>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string/join.hpp>
#include <toml.hpp>

#include <poac/core/except.hpp>
#include <poac/io/path.hpp>
#include <poac/util/cfg.hpp>

namespace poac::io::config {
    namespace detail {
        [[noreturn]] inline void
        rethrow_bad_cast(const std::string& what) {
            throw toml::type_error(
                    "[error] value type should be" +
                    what.substr(what.rfind(' ', what.find('\n'))));
        }

        [[noreturn]] inline void
        rethrow_cfg_exception(const util::cfg::exception& e, const toml::value& v) {
            const std::string what = e.what();
            std::vector<std::string> result;
            boost::algorithm::split(result, what, boost::is_any_of("\n"));

            std::string msg = result[0];
            const toml::source_location loc = v.location();
            msg += "\n --> " + loc.file_name() + "\n";

            const std::string line = std::to_string(loc.line()) + " ";
            const std::string line_str = line + "| " + loc.line_str();
            msg += line_str + "\n";

            msg += std::string(line.size(), ' ');
            msg +=  "|";
            msg += std::string(line_str.find(result[1]) - line.size() - 1, ' ');
            msg += result[2];
            throw util::cfg::exception(msg);
        }

        [[noreturn]] inline void
        rethrow_cfg_expr_error(const util::cfg::expression_error& e, const toml::value& v) {
            throw util::cfg::expression_error(toml::format_error(
                    "cfg expression error", v, e.what()));
        }

        //
        // find and force T type
        // If it cannot converted to T, throw exception.
        //
        template <typename T, typename C,
                template <typename...> class M,
                template <typename...> class V>
        decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        find_force(const toml::basic_value<C, M, V>& v, const toml::key& key) {
            try {
                return toml::find<T>(v, key);
            } catch (const toml::type_error& e) {
                rethrow_bad_cast(e.what());
            }
        }
        template <typename T, typename C,
                template <typename...> class M,
                template <typename...> class V>
        decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        find_force(toml::basic_value<C, M, V>& v, const toml::key& key) {
            try {
                return toml::find<T>(v, key);
            } catch (const toml::type_error& e) {
                rethrow_bad_cast(e.what());
            }
        }
        template <typename T, typename C,
                template <typename...> class M,
                template <typename...> class V>
        decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        find_force(toml::basic_value<C, M, V>&& v, const toml::key& key) {
            try {
                return toml::find<T>(std::move(v), key);
            } catch (const toml::type_error& e) {
                rethrow_bad_cast(e.what());
            }
        }

        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        find_force(const toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) {
            return find_force<T>(find_force(v, key), std::forward<Ts>(keys)...);
        }
        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        find_force(toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) {
            return find_force<T>(find_force(v, key), std::forward<Ts>(keys)...);
        }
        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        find_force(toml::basic_value<C, M, V>&& v, const toml::key& key, Ts&&... keys) {
            return find_force<T>(find_force(std::move(v), key), std::forward<Ts>(keys)...);
        }

        //
        // find and check possible values
        // If value cannot convert to T, or if value does not exist, throw exception.
        //
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        find_enum(const toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv) {
            const T value = find_force<T>(v, key);
            if (std::any_of(pv.cbegin(), pv.cend(), [&](T x){ return x == value; })) {
                return value;
            } else {
                std::vector<std::string> pvs(pv.size());
                std::transform(pv.cbegin(), pv.cend(), pvs.begin(), [](T x){ return std::to_string(x); });
                const auto f = "[error] value should be any of [" + boost::algorithm::join(pvs, ", ") + "]";
                throw toml::type_error(toml::format_error(
                        f, toml::get<toml::table>(v).at(key),
                        "one of the above listed is required"));
            }
        }
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        find_enum(toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv) {
            const T value = find_force<T>(v, key);
            if (std::any_of(pv.cbegin(), pv.cend(), [&](T x){ return x == value; })) {
                return value;
            } else {
                std::vector<std::string> pvs(pv.size());
                std::transform(pv.cbegin(), pv.cend(), pvs.begin(), [](T x){ return std::to_string(x); });
                const auto f = "[error] value should be any of [" + boost::algorithm::join(pvs, ", ") + "]";
                throw toml::type_error(toml::format_error(
                        f, toml::get<toml::table>(v).at(key),
                        "one of the above listed is required"));
            }
        }
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        find_enum(toml::basic_value<C, M, V>&& v, const toml::key& key, std::vector<T>&& pv) {
            const T value = find_force<T>(std::move(v), key);
            if (std::any_of(pv.cbegin(), pv.cend(), [&](T x){ return x == value; })) {
                return value;
            } else {
                std::vector<std::string> pvs(pv.size());
                std::transform(pv.cbegin(), pv.cend(), pvs.begin(), [](T x){ return std::to_string(x); });
                const auto f = "[error] value should be any of [" + boost::algorithm::join(pvs, ", ") + "]";
                throw toml::type_error(toml::format_error(
                        f, toml::get<toml::table>(v).at(key),
                        "one of the above listed is required"));
            }
        }

        //
        // find and check possible values
        // If it cannot converted to T, throw exception, and if the value does not exist, return std::nullopt.
        //
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >>
        find_enum_opt(const toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv) {
            try {
                return find_enum<T>(v, key, std::move(pv));
            } catch (const toml::type_error& e) {
                throw e;
            } catch (...) {
                return std::nullopt;
            }
        }
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >>
        find_enum_opt(toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv) {
            try {
                return find_enum<T>(v, key, std::move(pv));
            } catch (const toml::type_error& e) {
                throw e;
            } catch (...) {
                return std::nullopt;
            }
        }
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >>
        find_enum_opt(toml::basic_value<C, M, V>&& v, const toml::key& key, std::vector<T>&& pv) {
            try {
                return find_enum<T>(std::move(v), key, std::move(pv));
            } catch (const toml::type_error& e) {
                throw e;
            } catch (...) {
                return std::nullopt;
            }
        }

        //
        // find and force T type
        // If it cannot converted to T, throw exception, and if the value does not exist, return std::nullopt.
        //
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >>
        find_force_opt(const toml::basic_value<C, M, V>& v, const toml::key& key) {
            try {
                return find_force<T>(v, key);
            } catch (const toml::type_error& e) {
                throw e;
            } catch (...) {
                return std::nullopt;
            }
        }
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >>
        find_force_opt(toml::basic_value<C, M, V>& v, const toml::key& key) {
            try {
                return find_force<T>(v, key);
            } catch (const toml::type_error& e) {
                throw e;
            } catch (...) {
                return std::nullopt;
            }
        }
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >>
        find_force_opt(toml::basic_value<C, M, V>&& v, const toml::key& key) {
            try {
                return find_force<T>(std::move(v), key);
            } catch (const toml::type_error& e) {
                throw e;
            } catch (...) {
                return std::nullopt;
            }
        }

        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >>
        find_force_opt(const toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) {
            return find_force_opt<T>(find_force_opt(v, key), std::forward<Ts>(keys)...);
        }
        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >>
        find_force_opt(toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) {
            return find_force_opt<T>(find_force_opt(v, key), std::forward<Ts>(keys)...);
        }
        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >>
        find_force_opt(toml::basic_value<C, M, V>&& v, const toml::key& key, Ts&&... keys) {
            return find_force_opt<T>(find_force_opt(std::move(v), key), std::forward<Ts>(keys)...);
        }

        //
        // find as optional
        // If value cannot convert to T, or if value does not exist, return std::nullopt.
        //
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
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
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
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
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
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

        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >>
        find_opt(const toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) noexcept {
            return find_opt<T>(find_opt(v, key), std::forward<Ts>(keys)...);
        }
        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >>
        find_opt(toml::basic_value<C, M, V>& v, const toml::key& key, Ts&&... keys) noexcept {
            return find_opt<T>(find_opt(v, key), std::forward<Ts>(keys)...);
        }
        template <typename T, typename C, template <typename...> class M,
                template <typename...> class V, typename... Ts>
        std::optional<std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >>
        find_opt(toml::basic_value<C, M, V>&& v, const toml::key& key, Ts&&... keys) noexcept {
            return find_opt<T>(find_opt(std::move(v), key), std::forward<Ts>(keys)...);
        }

        inline boost::system::error_code ec{};

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

    // https://doc.poac.pm/en/reference/manifest.html#the-package-section
    struct Package {
        std::string name; // required
        std::string version; // required
        std::optional<std::vector<std::string>> authors; // optional
        std::uint16_t cpp; // optional with default(17)
        std::optional<std::string> build; // optional
        std::optional<std::string> links; // optional
        std::optional<std::vector<std::string>> exclude; // optional
        std::optional<std::vector<std::string>> include; // optional
        std::optional<toml::value> publish; // optional (bool or vec<string>) TODO: variant
        std::optional<std::string> workspace; // optional
        std::optional<std::string> description; // optional
        std::optional<std::string> documentation; // optional
        std::optional<std::string> homepage; // optional
        std::optional<std::string> repository; // optional
        std::optional<std::string> readme; // optional
        std::optional<std::string> license; // optional
        std::optional<std::string> license_file; // optional
        std::optional<std::string> default_run; // optional

        void from_toml(const toml::value& v) {
            name = detail::find_force<decltype(name)>(v, "name");
            version = detail::find_force<decltype(version)>(v, "version");
            authors = detail::find_force_opt<decltype(authors)::value_type>(v, "authors");
            cpp = detail::find_enum_opt<decltype(cpp)>(v, "cpp", {98, 3, 11, 14, 17, 20}).value_or(17);
            build = detail::find_force_opt<decltype(build)::value_type>(v, "build");
            links = detail::find_force_opt<decltype(links)::value_type>(v, "links");
            exclude = detail::find_force_opt<decltype(exclude)::value_type>(v, "exclude");
            include = detail::find_force_opt<decltype(include)::value_type>(v, "include");
            publish = detail::find_opt<decltype(publish)::value_type>(v, "publish");
            workspace = detail::find_force_opt<decltype(workspace)::value_type>(v, "workspace");
            description = detail::find_force_opt<decltype(description)::value_type>(v, "description");
            documentation = detail::find_force_opt<decltype(documentation)::value_type>(v, "documentation");
            homepage = detail::find_force_opt<decltype(homepage)::value_type>(v, "homepage");
            repository = detail::find_force_opt<decltype(repository)::value_type>(v, "repository");
            readme = detail::find_force_opt<decltype(readme)::value_type>(v, "readme");
            license = detail::find_force_opt<decltype(license)::value_type>(v, "license");
            license_file = detail::find_force_opt<decltype(license_file)::value_type>(v, "license-file");
            default_run = detail::find_force_opt<decltype(default_run)::value_type>(v, "default-run");
        }
        toml::table into_toml() const {
            toml::table t{};
            t.emplace("name", name);
            t.emplace("version", version);
            if (authors.has_value()) {
                t.emplace("authors", authors.value());
            }
            t.emplace("cpp", cpp);
            if (build.has_value()) {
                t.emplace("build", build.value());
            }
            if (links.has_value()) {
                t.emplace("links", links.value());
            }
            if (exclude.has_value()) {
                t.emplace("exclude", exclude.value());
            }
            if (include.has_value()) {
                t.emplace("include", include.value());
            }
            if (publish.has_value()) {
                t.emplace("publish", publish.value());
            }
            if (workspace.has_value()) {
                t.emplace("workspace", workspace.value());
            }
            if (description.has_value()) {
                t.emplace("description", description.value());
            }
            if (documentation.has_value()) {
                t.emplace("documentation", documentation.value());
            }
            if (homepage.has_value()) {
                t.emplace("homepage", homepage.value());
            }
            if (repository.has_value()) {
                t.emplace("repository", repository.value());
            }
            if (readme.has_value()) {
                t.emplace("readme", readme.value());
            }
            if (license.has_value()) {
                t.emplace("license", license.value());
            }
            if (license_file.has_value()) {
                t.emplace("license-file", license_file.value());
            }
            if (default_run.has_value()) {
                t.emplace("default-run", default_run.value());
            }
            return t;
        }
    };

    // https://doc.poac.pm/en/reference/manifest.html#the-profile-sections
    struct ProfileUnder {
        std::string opt_level; // optional with default
        bool debug; // optional with default
        bool lto; // optional with default
        bool incremental; // optional with default

        toml::table into_toml() const {
            return {
                { "opt-level", opt_level },
                { "debug", debug },
                { "lto", lto },
                { "incremental", incremental }
            };
        }
    };
    struct ProfileDev : ProfileUnder {
        void from_toml(const toml::value& v) {
            opt_level = detail::find_force_opt<decltype(opt_level)>(v, "opt-level").value_or("0");
            debug = detail::find_force_opt<decltype(debug)>(v, "debug").value_or(true);
            lto = detail::find_force_opt<decltype(lto)>(v, "lto").value_or(false);
            incremental = detail::find_force_opt<decltype(incremental)>(v, "incremental").value_or(true);
        }
    };
    struct ProfileRelease : ProfileUnder {
        void from_toml(const toml::value& v) {
            opt_level = detail::find_force_opt<decltype(opt_level)>(v, "opt-level").value_or("3");
            debug = detail::find_force_opt<decltype(debug)>(v, "debug").value_or(false);
            lto = detail::find_force_opt<decltype(lto)>(v, "lto").value_or(false);
            incremental = detail::find_force_opt<decltype(incremental)>(v, "incremental").value_or(false);
        }
    };
    struct Profile {
        std::optional<std::vector<std::string>> definitions; // optional
        std::optional<std::vector<std::string>> options; // optional
        std::optional<std::vector<std::string>> libraries; // optional
        std::optional<std::vector<std::string>> include_directories; // optional
        std::optional<std::vector<std::string>> link_directories; // optional
        std::optional<std::string> compiler; // optional
        std::optional<ProfileDev> dev; // optional
        std::optional<ProfileRelease> release; // optional
        std::optional<ProfileDev> test; // optional
        std::optional<ProfileRelease> bench; // optional

        void from_toml(const toml::value& v) {
            definitions = detail::find_force_opt<decltype(definitions)::value_type>(v, "definitions");
            options = detail::find_force_opt<decltype(options)::value_type>(v, "options");
            libraries = detail::find_force_opt<decltype(libraries)::value_type>(v, "libraries");
            include_directories = detail::find_force_opt<decltype(include_directories)::value_type>(v, "include-directories");
            link_directories = detail::find_force_opt<decltype(link_directories)::value_type>(v, "link-directories");
            compiler = detail::find_force_opt<decltype(compiler)::value_type>(v, "compiler");
            dev = detail::find_force_opt<decltype(dev)::value_type>(v, "dev");
            release = detail::find_force_opt<decltype(release)::value_type>(v, "release");
            test = detail::find_force_opt<decltype(test)::value_type>(v, "test");
            bench = detail::find_force_opt<decltype(bench)::value_type>(v, "bench");
        }
        toml::table into_toml() const {
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
            if (include_directories.has_value()) {
                t.emplace("include-directories", include_directories.value());
            }
            if (link_directories.has_value()) {
                t.emplace("link-directories", link_directories.value());
            }
            if (compiler.has_value()) {
                t.emplace("compiler", compiler.value());
            }
            if (dev.has_value()) {
                t.emplace("dev", dev.value());
            }
            if (release.has_value()) {
                t.emplace("release", release.value());
            }
            if (test.has_value()) {
                t.emplace("test", test.value());
            }
            if (bench.has_value()) {
                t.emplace("bench", bench.value());
            }
            return t;
        }
    };

    // https://doc.poac.pm/en/reference/manifest.html#configuring-a-target
    struct Bin {
        std::string name; // required
        std::string path; // required

        void from_toml(const toml::value& v) {
            name = detail::find_force<decltype(name)>(v, "name");
            path = detail::find_force<decltype(path)>(v, "path");
        }
        toml::table into_toml() const {
            return {
                { "name", name },
                { "path", path }
            };
        }
    };

    struct Config {
        Package package;
        std::optional<std::unordered_map<std::string, std::string>> dependencies;
        std::optional<std::unordered_map<std::string, std::string>> dev_dependencies;
        std::optional<std::unordered_map<std::string, std::string>> build_dependencies;
        std::optional<Profile> profile;
        std::optional<std::vector<Bin>> bin;

        void from_toml(const toml::value& v) {
            package = toml::find<decltype(package)>(v, "package");
            dependencies = detail::find_force_opt<decltype(dependencies)::value_type>(v, "dependencies");
            dev_dependencies = detail::find_force_opt<decltype(dev_dependencies)::value_type>(v, "dev-dependencies");
            build_dependencies = detail::find_force_opt<decltype(build_dependencies)::value_type>(v, "build-dependencies");
            profile = detail::find_force_opt<decltype(profile)::value_type>(v, "profile");
            bin = detail::find_force_opt<decltype(bin)::value_type>(v, "bin");

            const auto target = toml::find<toml::table>(v, "target");
            for (const auto& [key, value] : target) {
                try {
                    std::cout << "util::cfg::parse will parse " << key << std::endl;
                    if (util::cfg::parse(key).match()) {
                        std::cout << "match!" << std::endl;
                    } else {
                        std::cout << "unmatch..." << std::endl;
                    }
                    std::cout << std::endl;
                } catch (const util::cfg::expression_error& e) {
                    detail::rethrow_cfg_expr_error(e, target.at(key));
                } catch (const util::cfg::exception& e) {
                    detail::rethrow_cfg_exception(e, target.at(key));
                }

//                if (util::cfg::parse(key)) {
//                    const auto dependencies2 = detail::find_force_opt<decltype(profile)::value_type>(value, "profile");
//                    dependencies.value().insert(profile.value().end(), profile2.begin(), profile2.end());
//                    const auto profile2 = detail::find_force_opt<decltype(profile)::value_type>(value, "profile");
//                    profile.value().insert(profile.value().end(), profile2.begin(), profile2.end());
//                }
            }
        }
        toml::table into_toml() const {
            toml::table t{};
            t.emplace("package", package);
            if (dependencies.has_value()) {
                t.emplace("dependencies", dependencies.value());
            }
            if (dev_dependencies.has_value()) {
                t.emplace("dev-dependencies", dev_dependencies.value());
            }
            if (build_dependencies.has_value()) {
                t.emplace("build-dependencies", build_dependencies.value());
            }
            if (profile.has_value()) {
                t.emplace("properties", profile.value());
            }
            if (bin.has_value()) {
                t.emplace("bin", bin.value());
            }
            return t;
        }
    };

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
