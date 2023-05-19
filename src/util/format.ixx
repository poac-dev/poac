module;

// std
#include <string>
#include <utility>

// external
#include <fmt/core.h> // NOLINT(build/include_order)
#include <fmt/format.h> // NOLINT(build/include_order)
#include <fmt/std.h> // NOLINT(build/include_order)

export module poac.util.format;

export namespace poac {

//
// String literals
//
// using namespace fmt::literals;

//
// Utilities
//
using fmt::format;
using fmt::print;

} // namespace poac

//
// Custom formatters
//
#if FMT_VERSION < 90000

#  include <filesystem>
#  include <string_view>

namespace fmt {

template <>
struct formatter<std::string_view> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline auto format(std::string_view sv, FormatContext& ctx) {
    return format_to(ctx.out(), "{}", std::string(sv));
  }
};

template <>
struct formatter<std::filesystem::path> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline auto format(const std::filesystem::path& p, FormatContext& ctx) {
    return format_to(ctx.out(), "{}", p.string());
  }
};

template <typename T1, typename T2>
struct formatter<std::pair<T1, T2>> {
  constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

  template <typename FormatContext>
  inline auto format(const std::pair<T1, T2>& p, FormatContext& ctx) {
    return format_to(ctx.out(), "({}, {})", p.first, p.second);
  }
};

} // namespace fmt

#else

#  include <fmt/std.h>

#endif
