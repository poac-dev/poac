#ifndef SEMVER_IO_HPP
#define SEMVER_IO_HPP

#include <ostream> // std::ostream

#include <poac/util/semver/version.hpp>

namespace semver {
    std::ostream& operator<<(std::ostream& os, const Version& v) {
        os << "SemVer {\n";
        os << "  full: " << "\"" << v.get_full() << "\",\n";
        os << "  version: " << "\"" << v.get_version() << "\",\n";
        os << "  major: " << v.major << ",\n";
        os << "  minor: " << v.minor << ",\n";
        os << "  patch: " << v.patch << ",\n";
        os << "  prerelease: " << "[ ";
        for (const auto& s : v.pre) {
            os << "\"" << s << "\", ";
        }
        os << "],\n";
        os << "  build: " << "[ ";
        for (const auto& s : v.build) {
            os << "\"" << s << "\", ";
        }
        os << "],\n";
        os << "}";
        return os;
    }
} // end namespace semver

#endif // !SEMVER_IO_HPP
