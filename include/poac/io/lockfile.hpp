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
#include <poac/io/path.hpp>

namespace poac::io::lockfile {
    enum class PackageType {
        HeaderOnlyLib,
        BuildReqLib,
        Application
    };

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
    };
    using dependencies_type = std::unordered_map<std::string, Package>;
} // end namespace
#endif // !POAC_IO_LOCKFILE_HPP
