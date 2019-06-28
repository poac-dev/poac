#ifndef SEMVER_IO_HPP
#define SEMVER_IO_HPP

#include <ostream> // std::ostream

#include "version.hpp"

#include "../termcolor2.hpp"

namespace semver {
    std::ostream& operator<<(std::ostream& os, const Version& v) {
        os << "SemVer {\n";
        os << "  full: " << termcolor2::green<> << "\"" << v.get_full() << "\"" << termcolor2::reset<> << ",\n";
        os << "  version: " << termcolor2::green<> << "\"" << v.get_version() << "\"" << termcolor2::reset<> << ",\n";
        os << "  major: " << termcolor2::yellow<> << v.major << termcolor2::reset<> << ",\n";
        os << "  minor: " << termcolor2::yellow<> << v.minor << termcolor2::reset<> << ",\n";
        os << "  patch: " << termcolor2::yellow<> << v.patch << termcolor2::reset<> << ",\n";
        os << "  prerelease: " << "[ ";
        for (const auto& s : v.pre) {
            os << termcolor2::green<> << "\"" << s << "\"" << termcolor2::reset<> << ", ";
        }
        os << "],\n";
        os << "  build: " << "[ ";
        for (const auto& s : v.build) {
            os << termcolor2::green<> << "\"" << s << "\"" << termcolor2::reset<> << ", ";
        }
        os << "],\n";
        os << "}";
        return os;
    }
} // end namespace semver

#endif // !SEMVER_IO_HPP
