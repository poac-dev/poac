#ifndef POAC_IO_LOCKFILE_HPP
#define POAC_IO_LOCKFILE_HPP

#include <string>
#include <unordered_map>
#include <optional>

namespace poac::io::lockfile {
    struct Package {
        std::string version; // TODO: semver::Version
        std::optional<std::unordered_map<std::string, std::string>> dependencies;

        // std::unordered_map::operator[] needs default constructor.
        Package()
            : version("")
            , dependencies(std::nullopt)
        {}

        Package(
            const std::string& version,
            std::optional<std::unordered_map<std::string, std::string>> dependencies
        )
            : version(version)
            , dependencies(dependencies)
        {}

        explicit Package(const std::string& version)
            : version(version)
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
