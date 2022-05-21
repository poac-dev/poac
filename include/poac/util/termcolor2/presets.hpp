#ifndef POAC_UTIL_TERMCOLOR2_PRESETS_HPP_
#define POAC_UTIL_TERMCOLOR2_PRESETS_HPP_

// std
#include <concepts>
#include <string>
#include <type_traits>

// internal
#include <poac/util/termcolor2/config.hpp>

namespace termcolor2 {
namespace detail {

  // ref: https://zenn.dev/tetsurom/scraps/b6d81079559c91
#define CHAR_LITERAL(t, x) \
  ::termcolor2::detail::select_type<t>((x), (L##x), (u8##x), (u##x), (U##x))

  template <typename CharT, std::size_t N>
  [[nodiscard]] TERMCOLOR2_CXX20_CONSTEVAL_FN auto
  select_type(
      const char (&s1)[N], const wchar_t (&s2)[N], const char8_t (&s3)[N],
      const char16_t (&s4)[N], const char32_t (&s5)[N]
  ) noexcept -> const CharT (&)[N] {
    // C++23 > if consteval
    if constexpr (std::same_as<CharT, char>) {
      return s1;
    }
    if constexpr (std::same_as<CharT, wchar_t>) {
      return s2;
    }
    if constexpr (std::same_as<CharT, char8_t>) {
      return s3;
    }
    if constexpr (std::same_as<CharT, char16_t>) {
      return s4;
    }
    if constexpr (std::same_as<CharT, char32_t>) {
      return s5;
    }
  }

} // namespace detail

//
// Foreground manipulators
//
template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
gray_v() {
  return CHAR_LITERAL(CharT, "\x1b[30m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto gray = gray_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
red_v() {
  return CHAR_LITERAL(CharT, "\x1b[31m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto red = red_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
green_v() {
  return CHAR_LITERAL(CharT, "\x1b[32m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto green = green_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
yellow_v() {
  return CHAR_LITERAL(CharT, "\x1b[33m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto yellow = yellow_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
blue_v() {
  return CHAR_LITERAL(CharT, "\x1b[34m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto blue = blue_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
magenta_v() {
  return CHAR_LITERAL(CharT, "\x1b[35m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto magenta = magenta_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
cyan_v() {
  return CHAR_LITERAL(CharT, "\x1b[36m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto cyan = cyan_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
white_v() {
  return CHAR_LITERAL(CharT, "\x1b[37m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto white = white_v();

//
// Background manipulators
//
template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
on_gray_v() {
  return CHAR_LITERAL(CharT, "\x1b[40m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_gray = on_gray_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
on_red_v() {
  return CHAR_LITERAL(CharT, "\x1b[41m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_red = on_red_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
on_green_v() {
  return CHAR_LITERAL(CharT, "\x1b[42m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_green = on_green_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
on_yellow_v() {
  return CHAR_LITERAL(CharT, "\x1b[43m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_yellow = on_yellow_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
on_blue_v() {
  return CHAR_LITERAL(CharT, "\x1b[44m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_blue = on_blue_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
on_magenta_v() {
  return CHAR_LITERAL(CharT, "\x1b[45m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_magenta = on_magenta_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
on_cyan_v() {
  return CHAR_LITERAL(CharT, "\x1b[46m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_cyan = on_cyan_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
on_white_v() {
  return CHAR_LITERAL(CharT, "\x1b[47m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_white = on_white_v();

//
// Attribute manipulators
//
template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
bold_v() {
  return CHAR_LITERAL(CharT, "\x1b[1m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto bold = bold_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
dark_v() {
  return CHAR_LITERAL(CharT, "\x1b[2m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto dark = dark_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
underline_v() {
  return CHAR_LITERAL(CharT, "\x1b[4m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto underline = underline_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
blink_v() {
  return CHAR_LITERAL(CharT, "\x1b[5m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto blink = blink_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
reverse_v() {
  return CHAR_LITERAL(CharT, "\x1b[7m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto reverse = reverse_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
concealed_v() {
  return CHAR_LITERAL(CharT, "\x1b[8m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto concealed = concealed_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN std::basic_string<CharT>
reset_v() {
  return CHAR_LITERAL(CharT, "\x1b[0m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto reset = reset_v();

} // end namespace termcolor2

#endif // POAC_UTIL_TERMCOLOR2_PRESETS_HPP_
