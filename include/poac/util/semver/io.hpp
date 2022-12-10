#ifndef POAC_UTIL_SEMVER_IO_HPP_
#define POAC_UTIL_SEMVER_IO_HPP_

// std
#include <ostream> // std::ostream

// internal
#include "poac/util/semver/token.hpp"

namespace semver {

inline auto operator<<(std::ostream& os, const Version& v) -> std::ostream& {
  os << v.get_full();
  return os;
}

} // end namespace semver

namespace fmt {

template <>
struct formatter<semver::Version> {
  static constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline auto format(const semver::Version& v, FormatContext& ctx) {
    return format_to(ctx.out(), "{}", v.get_full());
  }
};

} // namespace fmt

#endif // POAC_UTIL_SEMVER_IO_HPP_
