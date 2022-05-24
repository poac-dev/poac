#ifndef POAC_UTIL_SEMVER_COMPARISON_HPP_
#define POAC_UTIL_SEMVER_COMPARISON_HPP_

// std
#include <algorithm>
#include <string>

// internal
#include "poac/util/semver/lexer.hpp"
#include "poac/util/semver/parser.hpp"
#include "poac/util/semver/token.hpp"

namespace semver {
namespace detail {

  bool
  gt_pre(const Version& lhs, const Version& rhs);
  bool
  eq_pre(const Version& lhs, const Version& rhs);

} // end namespace detail

inline bool
operator==(const Version& lhs, const Version& rhs) {
  return lhs.major == rhs.major && lhs.minor == rhs.minor &&
         lhs.patch == rhs.patch && detail::eq_pre(lhs, rhs);
}
inline bool
operator==(const Version& lhs, const std::string& rhs) {
  return lhs == parse(rhs);
}
inline bool
operator==(const std::string& lhs, const Version& rhs) {
  return parse(lhs) == rhs;
}
inline bool
operator==(const Version& lhs, const char* rhs) {
  return lhs == parse(rhs);
}
inline bool
operator==(const char* lhs, const Version& rhs) {
  return parse(lhs) == rhs;
}

inline bool
operator!=(const Version& lhs, const Version& rhs) {
  return !(lhs == rhs);
}
inline bool
operator!=(const Version& lhs, const std::string& rhs) {
  return !(lhs == rhs);
}
inline bool
operator!=(const std::string& lhs, const Version& rhs) {
  return !(lhs == rhs);
}
inline bool
operator!=(const Version& lhs, const char* rhs) {
  return !(lhs == rhs);
}
inline bool
operator!=(const char* lhs, const Version& rhs) {
  return !(lhs == rhs);
}

bool
operator>(const Version& lhs, const Version& rhs); // gt
inline bool
operator>(const Version& lhs, const std::string& rhs) {
  return lhs > parse(rhs);
}
inline bool
operator>(const std::string& lhs, const Version& rhs) {
  return parse(lhs) > rhs;
}
inline bool
operator>(const Version& lhs, const char* rhs) {
  return lhs > parse(rhs);
}
inline bool
operator>(const char* lhs, const Version& rhs) {
  return parse(lhs) > rhs;
}

inline bool
operator<(const Version& lhs, const Version& rhs) {
  return rhs > lhs;
}
inline bool
operator<(const Version& lhs, const std::string& rhs) {
  return rhs > lhs;
}
inline bool
operator<(const std::string& lhs, const Version& rhs) {
  return rhs > lhs;
}
inline bool
operator<(const Version& lhs, const char* rhs) {
  return rhs > lhs;
}
inline bool
operator<(const char* lhs, const Version& rhs) {
  return rhs > lhs;
}

inline bool
operator>=(const Version& lhs, const Version& rhs) {
  return lhs > rhs || lhs == rhs;
}
inline bool
operator>=(const Version& lhs, const std::string& rhs) {
  return lhs > rhs || lhs == rhs;
}
inline bool
operator>=(const std::string& lhs, const Version& rhs) {
  return lhs > rhs || lhs == rhs;
}
inline bool
operator>=(const Version& lhs, const char* rhs) {
  return lhs > rhs || lhs == rhs;
}
inline bool
operator>=(const char* lhs, const Version& rhs) {
  return lhs > rhs || lhs == rhs;
}

inline bool
operator<=(const Version& lhs, const Version& rhs) {
  return lhs < rhs || lhs == rhs;
}
inline bool
operator<=(const Version& lhs, const std::string& rhs) {
  return lhs < rhs || lhs == rhs;
}
inline bool
operator<=(const std::string& lhs, const Version& rhs) {
  return lhs < rhs || lhs == rhs;
}
inline bool
operator<=(const Version& lhs, const char* rhs) {
  return lhs < rhs || lhs == rhs;
}
inline bool
operator<=(const char* lhs, const Version& rhs) {
  return lhs < rhs || lhs == rhs;
}

} // end namespace semver

#endif // POAC_UTIL_SEMVER_COMPARISON_HPP_
