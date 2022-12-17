#pragma once

// std
#include <algorithm>
#include <string>

// internal
#include "poac/util/semver/lexer.hpp"
#include "poac/util/semver/parser.hpp"
#include "poac/util/semver/token.hpp"

namespace semver {
namespace detail {

  auto gt_pre(const Version& lhs, const Version& rhs) -> bool;
  auto eq_pre(const Version& lhs, const Version& rhs) -> bool;

} // end namespace detail

inline auto operator==(const Version& lhs, const Version& rhs) -> bool {
  return lhs.major == rhs.major && lhs.minor == rhs.minor
         && lhs.patch == rhs.patch && detail::eq_pre(lhs, rhs);
}
inline auto operator==(const Version& lhs, const std::string& rhs) -> bool {
  return lhs == parse(rhs);
}
inline auto operator==(const std::string& lhs, const Version& rhs) -> bool {
  return parse(lhs) == rhs;
}
inline auto operator==(const Version& lhs, const char* rhs) -> bool {
  return lhs == parse(rhs);
}
inline auto operator==(const char* lhs, const Version& rhs) -> bool {
  return parse(lhs) == rhs;
}

inline auto operator!=(const Version& lhs, const Version& rhs) -> bool {
  return !(lhs == rhs);
}
inline auto operator!=(const Version& lhs, const std::string& rhs) -> bool {
  return !(lhs == rhs);
}
inline auto operator!=(const std::string& lhs, const Version& rhs) -> bool {
  return !(lhs == rhs);
}
inline auto operator!=(const Version& lhs, const char* rhs) -> bool {
  return !(lhs == rhs);
}
inline auto operator!=(const char* lhs, const Version& rhs) -> bool {
  return !(lhs == rhs);
}

auto operator>(const Version& lhs, const Version& rhs) -> bool; // gt
inline auto operator>(const Version& lhs, const std::string& rhs) -> bool {
  return lhs > parse(rhs);
}
inline auto operator>(const std::string& lhs, const Version& rhs) -> bool {
  return parse(lhs) > rhs;
}
inline auto operator>(const Version& lhs, const char* rhs) -> bool {
  return lhs > parse(rhs);
}
inline auto operator>(const char* lhs, const Version& rhs) -> bool {
  return parse(lhs) > rhs;
}

inline auto operator<(const Version& lhs, const Version& rhs) -> bool {
  return rhs > lhs;
}
inline auto operator<(const Version& lhs, const std::string& rhs) -> bool {
  return rhs > lhs;
}
inline auto operator<(const std::string& lhs, const Version& rhs) -> bool {
  return rhs > lhs;
}
inline auto operator<(const Version& lhs, const char* rhs) -> bool {
  return rhs > lhs;
}
inline auto operator<(const char* lhs, const Version& rhs) -> bool {
  return rhs > lhs;
}

inline auto operator>=(const Version& lhs, const Version& rhs) -> bool {
  return lhs > rhs || lhs == rhs;
}
inline auto operator>=(const Version& lhs, const std::string& rhs) -> bool {
  return lhs > rhs || lhs == rhs;
}
inline auto operator>=(const std::string& lhs, const Version& rhs) -> bool {
  return lhs > rhs || lhs == rhs;
}
inline auto operator>=(const Version& lhs, const char* rhs) -> bool {
  return lhs > rhs || lhs == rhs;
}
inline auto operator>=(const char* lhs, const Version& rhs) -> bool {
  return lhs > rhs || lhs == rhs;
}

inline auto operator<=(const Version& lhs, const Version& rhs) -> bool {
  return lhs < rhs || lhs == rhs;
}
inline auto operator<=(const Version& lhs, const std::string& rhs) -> bool {
  return lhs < rhs || lhs == rhs;
}
inline auto operator<=(const std::string& lhs, const Version& rhs) -> bool {
  return lhs < rhs || lhs == rhs;
}
inline auto operator<=(const Version& lhs, const char* rhs) -> bool {
  return lhs < rhs || lhs == rhs;
}
inline auto operator<=(const char* lhs, const Version& rhs) -> bool {
  return lhs < rhs || lhs == rhs;
}

} // end namespace semver
