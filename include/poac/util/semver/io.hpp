#ifndef POAC_UTIL_SEMVER_IO_HPP_
#define POAC_UTIL_SEMVER_IO_HPP_

// std
#include <ostream> // std::ostream

// internal
#include "poac/util/semver/token.hpp"

namespace semver {

inline std::ostream&
operator<<(std::ostream& os, const Version& v) {
  os << v.get_full();
  return os;
}

} // end namespace semver

#endif // POAC_UTIL_SEMVER_IO_HPP_
