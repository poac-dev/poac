#ifndef SEMVER_IO_HPP
#define SEMVER_IO_HPP

#include <ostream> // std::ostream

#include <poac/util/semver/version.hpp>

namespace semver {
    std::ostream& operator<<(std::ostream& os, const Version& v) {
        os << v.get_full();
        return os;
    }
} // end namespace semver

#endif // !SEMVER_IO_HPP
