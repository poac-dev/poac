#ifndef POAC_IO_LOCKFILE_HPP
#define POAC_IO_LOCKFILE_HPP

#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <optional>
#include <vector>

#include <toml.hpp>

#include <poac/core/except.hpp>
#include <poac/io/config.hpp>
#include <poac/io/path.hpp>

namespace poac::io::lockfile {
    enum class PackageType {
        HeaderOnlyLib,
        BuildReqLib,
        Application
    };

    std::string
    to_string(PackageType package_type) {
        switch (package_type) {
            case PackageType::HeaderOnlyLib:
                return "header-only library";
            case PackageType::BuildReqLib:
                return "build-required library";
            case PackageType::Application:
                return "application";
            default:
                throw std::logic_error(
                        "To access out of range of the "
                        "enumeration values is undefined behavior.");
        }
    }

    std::optional<PackageType>
    to_package_type(std::string_view str) noexcept {
        if (str == "header-only library") {
            return PackageType::HeaderOnlyLib;
        } else if (str == "build-required library") {
            return PackageType::BuildReqLib;
        } else if (str == "application") {
            return PackageType::Application;
        } else {
            return std::nullopt;
        }
    }

    struct Lockfile {
        struct Package {
            std::string version; // TODO: semver::Version
            PackageType package_type;
            std::optional<std::unordered_map<std::string, std::string>> dependencies;

            // std::unordered_map::operator[] needs default constructor.
            Package()
                : version("")
                , package_type(PackageType::HeaderOnlyLib)
                , dependencies(std::nullopt)
            {}

            Package(
                const std::string& version,
                PackageType package_type,
                std::optional<std::unordered_map<std::string, std::string>> dependencies
            )
                : version(version)
                , package_type(package_type)
                , dependencies(dependencies)
            {}

            explicit Package(const std::string& version)
                : version(version)
                , package_type(PackageType::HeaderOnlyLib)
                , dependencies(std::nullopt)
            {}

            ~Package() = default;
            Package(const Package&) = default;
            Package& operator=(const Package&) = default;
            Package(Package&&) noexcept = default;
            Package& operator=(Package&&) noexcept = default;

            template <typename C, template <typename ...> class M, template <typename ...> class V>
            void from_toml(const toml::basic_value<C, M, V>& v);
        };
        using dependencies_type = std::unordered_map<std::string, Package>;
        std::string timestamp;
        dependencies_type dependencies;

        template <typename C, template <typename ...> class M, template <typename ...> class V>
        void from_toml(const toml::basic_value<C, M, V>& v);
    };

    template <typename C, template <typename ...> class M, template <typename ...> class V>
    void Lockfile::from_toml(const toml::basic_value<C, M, V>& v) {
        timestamp = toml::find<std::string>(v, "timestamp");
        dependencies = toml::find<Lockfile::dependencies_type>(v, "dependencies");
    }

    template <typename C, template <typename ...> class M, template <typename ...> class V>
    void Lockfile::Package::from_toml(const toml::basic_value<C, M, V>& v) {
        version = toml::find<std::string>(v, "version");
        package_type = to_package_type(toml::find<std::string>(v, "package-type")).value();
        dependencies = config::detail::find_opt<std::unordered_map<std::string, std::string>>(v, "dependencies");
    }
} // end namespace
#endif // !POAC_IO_LOCKFILE_HPP
