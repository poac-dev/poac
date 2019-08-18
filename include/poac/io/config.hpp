#ifndef POAC_IO_CONFIG_HPP
#define POAC_IO_CONFIG_HPP

#include <algorithm>
#include <string>
#include <string_view>
#include <sstream>
#include <unordered_map>
#include <optional>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <toml.hpp>

#include <poac/core/except.hpp>
#include <poac/io/filesystem.hpp>
#include <poac/util/cfg.hpp>
#include <poac/util/semver/semver.hpp>
#include <poac/util/types.hpp>

namespace poac::io::config {
    namespace detail {
        [[noreturn]] inline void
        rethrow_bad_cast(const std::string& what) {
            throw toml::type_error(
                    "[error] value type should be" +
                    what.substr(what.rfind(' ', what.find('\n'))));
        }

        [[noreturn]] void
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

        // std::vector<T>{"one", "two"} -> std::string["one", "two"]
        // std::vector<T>{1, 2, 3} -> std::string[1, 2, 3]
        template <typename T>
        inline std::string
        to_toml_array_string(std::vector<T>&& v) {
            if constexpr (std::is_same_v<T, std::string>) {
                return "[" +
                        boost::algorithm::join(std::move(v)
                        | boost::adaptors::transformed(
                            [](T x){
                                std::stringstream ss;
                                ss << std::quoted(x);
                                return ss.str();
                            })
                        , ", ") + "]";
            } else {
                return "[" +
                        boost::algorithm::join(std::move(v)
                        | boost::adaptors::transformed(
                            [](T x){ return std::to_string(x); })
                        , ", ") + "]";
            }
        }

        //
        // find and check possible values
        // If value cannot convert to T, or if value does not exist, throw exception.
        //
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::remove_reference_t<
            decltype(toml::get<T>(std::declval<const toml::basic_value<C, M, V>&>()))
        >
        find_enum(const toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv) {
            const T value = find_force<T>(v, key);
            if (std::any_of(pv.cbegin(), pv.cend(), [&](T x){ return x == value; })) {
                return value;
            } else {
                const auto f = "[error] value should be any of " + to_toml_array_string(std::move(pv));
                throw toml::type_error(toml::format_error(
                        f, toml::get<toml::table>(v).at(key),
                        "one of the above listed is required"));
            }
        }
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&>()))
        >
        find_enum(toml::basic_value<C, M, V>& v, const toml::key& key, std::vector<T>&& pv) {
            const T value = find_force<T>(v, key);
            if (std::any_of(pv.cbegin(), pv.cend(), [&](T x){ return x == value; })) {
                return value;
            } else {
                const auto f = "[error] value should be any of " + to_toml_array_string(std::move(pv));
                throw toml::type_error(toml::format_error(
                        f, toml::get<toml::table>(v).at(key),
                        "one of the above listed is required"));
            }
        }
        template <typename T, typename C, template <typename...> class M, template <typename...> class V>
        std::remove_reference_t<
            decltype(toml::get<T>(std::declval<toml::basic_value<C, M, V>&&>()))
        >
        find_enum(toml::basic_value<C, M, V>&& v, const toml::key& key, std::vector<T>&& pv) {
            const T value = find_force<T>(std::move(v), key);
            if (std::any_of(pv.cbegin(), pv.cend(), [&](T x){ return x == value; })) {
                return value;
            } else {
                const auto f = "[error] value should be any of " + to_toml_array_string(std::move(pv));
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

        template <typename Field>
        inline void
        field_from_toml(Field& f, const toml::value& v, const toml::key& k) {
            f = detail::find_force<Field>(v, k);
        }
        template <typename Field>
        inline void
        field_from_toml(std::optional<Field>& f, const toml::value& v, const toml::key& k) {
            f = detail::find_force_opt<Field>(v, k);
        }
        template <typename Field, typename Default>
        inline void
        field_from_toml(Field& f, const toml::value& v, const toml::key& k, Default&& d) {
            f = detail::find_force_opt<Field>(v, k).value_or(d);
        }

        template <typename Field,
            std::enable_if_t<
                std::is_same_v<
                    Field,
                    std::vector<
                        typename Field::value_type,
                        typename Field::allocator_type
                    >
                >,
                std::nullptr_t
            > = nullptr>
        inline void
        merge(Field& f, const Field& f2) {
            f.insert(f.end(), f2.cbegin(), f2.cend());
        }
        template <typename Field,
            std::enable_if_t<
                std::is_same_v<
                    Field,
                    std::unordered_map<
                        typename Field::key_type,
                        typename Field::mapped_type
                    >
                >,
                std::nullptr_t
            > = nullptr>
        inline void
        merge(Field& f, const Field& f2) {
            f.insert(f2.cbegin(), f2.cend());
        }
        template <typename Field>
        inline auto
        merge(Field& f, const Field& f2) -> decltype(f.merge(f2), void()) {
            f.merge(f2);
        }
        template <typename Field>
        inline void
        merge(std::optional<Field>& f, const std::optional<Field>& f2) {
            if (f2.has_value()) {
                if (f.has_value()) {
                    merge(f.value(), f2.value());
                } else {
                    f = f2;
                }
            }
        }
        template <typename Field>
        inline void
        merge(std::optional<Field>& f, const toml::value& v, const toml::key& k) {
            merge(f, detail::find_force_opt<Field>(v, k));
        }

        std::optional<std::string>
        validate_config(const io::filesystem::path& base = filesystem::current) {
            const auto config_path = base / "poac.toml";
            if (io::filesystem::exists(config_path)) {
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
            detail::field_from_toml(this->name, v, "name");
            this->version = semver::Version(detail::find_force<std::string>(v, "version")).get_full();
            detail::field_from_toml(this->authors, v, "authors");
            this->cpp = detail::find_enum_opt<decltype(this->cpp)>(v, "cpp", {98, 3, 11, 14, 17, 20}).value_or(17);
            detail::field_from_toml(this->build, v, "build");
            detail::field_from_toml(this->links, v, "links");
            detail::field_from_toml(this->exclude, v, "exclude");
            detail::field_from_toml(this->include, v, "include");
            this->publish = detail::find_opt<decltype(this->publish)::value_type>(v, "publish");
            detail::field_from_toml(this->workspace, v, "workspace");
            detail::field_from_toml(this->description, v, "description");
            detail::field_from_toml(this->documentation, v, "documentation");
            detail::field_from_toml(this->homepage, v, "homepage");
            detail::field_from_toml(this->repository, v, "repository");
            detail::field_from_toml(this->readme, v, "readme");
            detail::field_from_toml(this->license, v, "license");
            detail::field_from_toml(this->license_file, v, "license-file");
            detail::field_from_toml(this->default_run, v, "default-run");
        }
    };

    // https://doc.poac.pm/en/reference/manifest.html#the-profile-sections
    struct ProfileBase {
        ProfileBase() = default;
        virtual ~ProfileBase() = default;
        ProfileBase(const ProfileBase&) = default;
        ProfileBase& operator=(const ProfileBase&) = default;
        ProfileBase(ProfileBase&&) noexcept = default;
        ProfileBase& operator=(ProfileBase&&) noexcept = default;

        std::optional<std::vector<std::string>> definitions; // optional (merge)
        std::optional<std::vector<std::string>> options; // optional (merge)
        std::optional<std::vector<std::string>> libraries; // optional (merge)
        std::optional<std::vector<std::string>> include_directories; // optional (merge)
        std::optional<std::vector<std::string>> link_directories; // optional (merge)
        std::optional<std::string> compiler; // optional (overwrite)
        std::string opt_level; // optional with default (overwrite)
        bool debug; // optional with default (overwrite)
        bool lto; // optional with default (overwrite)
        bool incremental; // optional with default (overwrite)

        virtual void from_toml(const toml::value& v) {
            detail::field_from_toml(this->definitions, v, "definitions");
            detail::field_from_toml(this->options, v, "options");
            detail::field_from_toml(this->libraries, v, "libraries");
            detail::field_from_toml(this->include_directories, v, "include-directories");
            detail::field_from_toml(this->link_directories, v, "link-directories");
            detail::field_from_toml(this->compiler, v, "compiler");
        }
        virtual void merge(const ProfileBase& profile) {
            detail::merge(this->definitions, profile.definitions);
            detail::merge(this->options, profile.options);
            detail::merge(this->libraries, profile.libraries);
            detail::merge(this->include_directories, profile.include_directories);
            detail::merge(this->link_directories, profile.link_directories);
            if (profile.compiler.has_value()) {
                this->compiler = profile.compiler;
            }
        }
    };
    struct ProfileDev : public ProfileBase {
        void from_toml(const toml::value& v) override {
            ProfileBase::from_toml(v);
            detail::field_from_toml(this->opt_level, v, "opt-level", "0"); // TODO: 0, 1, 2, 3, g, s => enum
            detail::field_from_toml(this->debug, v, "debug", true);
            detail::field_from_toml(this->lto, v, "lto", false);
            detail::field_from_toml(this->incremental, v, "incremental", true);
        }
        void merge(const ProfileBase& profile) override {
            ProfileBase::merge(profile);
            if (this->opt_level == "0" && profile.opt_level != "0") {
                this->opt_level = profile.opt_level;
            }
            if (this->debug && !profile.debug) {
                this->debug = profile.debug;
            }
            if (!this->lto && profile.lto) {
                this->lto = profile.lto;
            }
            if (this->incremental && !profile.incremental) {
                this->incremental = profile.incremental;
            }
        }
    };
    struct ProfileRelease : public ProfileBase {
        void from_toml(const toml::value& v) override {
            ProfileBase::from_toml(v);
            detail::field_from_toml(this->opt_level, v, "opt-level", "3");
            detail::field_from_toml(this->debug, v, "debug", false);
            detail::field_from_toml(this->lto, v, "lto", false);
            detail::field_from_toml(this->incremental, v, "incremental", false);
        }
        void merge(const ProfileBase& profile) override {
            ProfileBase::merge(profile);
            if (this->opt_level == "3" && profile.opt_level != "3") {
                this->opt_level = profile.opt_level;
            }
            if (!this->debug && profile.debug) {
                this->debug = profile.debug;
            }
            if (!this->lto && profile.lto) {
                this->lto = profile.lto;
            }
            if (!this->incremental && profile.incremental) {
                this->incremental = profile.incremental;
            }
        }
    };
    struct Profile {
        std::optional<ProfileDev> dev; // optional
        std::optional<ProfileRelease> release; // optional
        std::optional<ProfileDev> test; // optional
        std::optional<ProfileRelease> bench; // optional

        void from_toml(const toml::value& v) {
            this->dev = ProfileDev{};
            detail::field_from_toml(this->dev->definitions, v, "definitions");
            detail::field_from_toml(this->dev->options, v, "options");
            detail::field_from_toml(this->dev->libraries, v, "libraries");
            detail::field_from_toml(this->dev->include_directories, v, "include-directories");
            detail::field_from_toml(this->dev->link_directories, v, "link-directories");
            detail::field_from_toml(this->dev->compiler, v, "compiler");
            detail::field_from_toml(this->dev, v, "dev");

            this->release = ProfileRelease{};
            detail::field_from_toml(this->release->definitions, v, "definitions");
            detail::field_from_toml(this->release->options, v, "options");
            detail::field_from_toml(this->release->libraries, v, "libraries");
            detail::field_from_toml(this->release->include_directories, v, "include-directories");
            detail::field_from_toml(this->release->link_directories, v, "link-directories");
            detail::field_from_toml(this->release->compiler, v, "compiler");
            detail::field_from_toml(this->release, v, "release");

            this->test = ProfileDev{};
            detail::field_from_toml(this->test->definitions, v, "definitions");
            detail::field_from_toml(this->test->options, v, "options");
            detail::field_from_toml(this->test->libraries, v, "libraries");
            detail::field_from_toml(this->test->include_directories, v, "include-directories");
            detail::field_from_toml(this->test->link_directories, v, "link-directories");
            detail::field_from_toml(this->test->compiler, v, "compiler");
            detail::field_from_toml(this->test, v, "test");

            this->bench = ProfileRelease{};
            detail::field_from_toml(this->bench->definitions, v, "definitions");
            detail::field_from_toml(this->bench->options, v, "options");
            detail::field_from_toml(this->bench->libraries, v, "libraries");
            detail::field_from_toml(this->bench->include_directories, v, "include-directories");
            detail::field_from_toml(this->bench->link_directories, v, "link-directories");
            detail::field_from_toml(this->bench->compiler, v, "compiler");
            detail::field_from_toml(this->bench, v, "bench");
        }

        void merge(const Profile& profile) {
            detail::merge(this->dev, profile.dev);
            detail::merge(this->release, profile.release);
            detail::merge(this->test, profile.test);
            detail::merge(this->bench, profile.bench);
        }
    };

    // https://doc.poac.pm/en/reference/manifest.html#configuring-a-target
    struct Bin {
        std::string name; // required
        std::string path; // required

        void from_toml(const toml::value& v) {
            detail::field_from_toml(this->name, v, "name");
            detail::field_from_toml(this->path, v, "path");
        }
    };

    struct Config {
    private:
        using dependencies_type = std::unordered_map<std::string, std::string>;

    public:
        Package package; // required
        std::optional<dependencies_type> dependencies; // optional and platform-specific field (merge)
        std::optional<dependencies_type> dev_dependencies; // optional and platform-specific field (merge)
        std::optional<dependencies_type> build_dependencies; // optional and platform-specific field (merge)
        std::optional<Profile> profile; // optional and platform-specific field (merge)
        std::optional<std::vector<Bin>> bin; // optional

        void from_toml(const toml::value& v) {
            detail::field_from_toml(this->package, v, "package");
            detail::field_from_toml(this->dependencies, v, "dependencies");
            detail::field_from_toml(this->dev_dependencies, v, "dev-dependencies");
            detail::field_from_toml(this->build_dependencies, v, "build-dependencies");
            detail::field_from_toml(this->profile, v, "profile");
            detail::field_from_toml(this->bin, v, "bin");
            const auto target = detail::find_opt<toml::table>(v, "target");
            if (target.has_value()) {
                for (const auto& [key, value] : target.value()) {
                    try {
                        if (util::cfg::parse(key).match()) {
                            detail::merge(this->dependencies, value, "dependencies");
                            detail::merge(this->dev_dependencies, value, "dev-dependencies");
                            detail::merge(this->build_dependencies, value, "build-dependencies");
                            detail::merge(this->profile, value, "profile");
                        }
                    } catch (const util::cfg::expression_error& e) {
                        detail::rethrow_cfg_expr_error(e, target->at(key));
                    } catch (const util::cfg::exception& e) {
                        detail::rethrow_cfg_exception(e, target->at(key));
                    }
                }
            }
        }
    };

    template <typename C>
    std::optional<C>
    load_toml(const io::filesystem::path& base, const std::string& fname) {
        if (io::filesystem::exists(base / fname)) {
            const auto config_toml = toml::parse<toml::preserve_comments>((base / fname).string());
            const auto config = toml::get<C>(config_toml);
            return config;
        } else {
            return std::nullopt;
        }
    }

    std::optional<Config>
    load(const io::filesystem::path& base = filesystem::current) {
         return load_toml<Config>(base, "poac.toml");
    }

    std::string
    get_timestamp() {
        if (const auto filename = config::detail::validate_config()) {
            const auto last_time = io::filesystem::last_write_time(filename.value());
            return io::filesystem::time_to_string(last_time);
        } else {
            throw core::except::error(
                    core::except::msg::does_not_exist("poac.toml"), "\n",
                    core::except::msg::please_exec("`poac init` or `poac new $PROJNAME`"));
        }
    }
} // end namespace
#endif // !POAC_IO_CONFIG_HPP
