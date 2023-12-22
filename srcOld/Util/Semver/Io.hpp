module;

// std
#include <ostream> // std::ostream

// external
#include <fmt/format.h>

export module semver.io;

import semver.token;

namespace semver {

inline auto operator<<(std::ostream& os, const Version& v) -> std::ostream& {
  os << v.get_full();
  return os;
}

} // end namespace semver

namespace fmt {

template <>
struct formatter<semver::Version> {
  static constexpr auto parse(format_parse_context& ctx) {
    return ctx.begin();
  }

  template <typename FormatContext>
  inline auto format(const semver::Version& v, FormatContext& ctx) {
    return format_to(ctx.out(), "{}", v.get_full());
  }
};

} // namespace fmt
