#ifndef SEMVER_IO_HPP
#define SEMVER_IO_HPP

// std
#include <ostream> // std::ostream

// internal
#include <poac/util/semver/parser/token.hpp>

namespace semver {

std::ostream&
operator<<(std::ostream& os, const Version& v) {
  os << v.get_full();
  return os;
}

} // end namespace semver

#endif // !SEMVER_IO_HPP
