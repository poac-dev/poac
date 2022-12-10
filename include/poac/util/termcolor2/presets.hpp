#ifndef POAC_UTIL_TERMCOLOR2_PRESETS_HPP_
#define POAC_UTIL_TERMCOLOR2_PRESETS_HPP_

// std
#include <string>
#include <type_traits>

// internal
#include "poac/util/termcolor2/config.hpp"

namespace termcolor2 {
namespace detail {

  // ref: https://zenn.dev/tetsurom/scraps/b6d81079559c91
#define CHAR_LITERAL(t, x) \
  ::termcolor2::detail::select_type<t>((x), (L##x), (u8##x), (u##x), (U##x))

  template <typename CharT, std::size_t N>
  [[nodiscard]] TERMCOLOR2_CXX20_CONSTEVAL_FN auto select_type(
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      const char (&s1)[N], const wchar_t (&s2)[N], const char8_t (&s3)[N],
      // NOLINTNEXTLINE(modernize-avoid-c-arrays)
      const char16_t (&s4)[N], const char32_t (&s5)[N]
  ) noexcept -> std::basic_string<CharT> {
    // C++23 > if consteval
    try {
      if constexpr (std::is_same_v<CharT, char>) {
        return s1;
      }
      if constexpr (std::is_same_v<CharT, wchar_t>) {
        return s2;
      }
      if constexpr (std::is_same_v<CharT, char8_t>) {
        return s3;
      }
      if constexpr (std::is_same_v<CharT, char16_t>) {
        return s4;
      }
      if constexpr (std::is_same_v<CharT, char32_t>) {
        return s5;
      }
    } catch (...) {
      __builtin_unreachable();
    }
  }

} // namespace detail

//
// Foreground manipulators
//
template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto gray_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[30m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto gray = gray_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto red_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[31m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto red = red_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto green_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[32m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto green = green_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto yellow_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[33m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto yellow = yellow_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto blue_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[34m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto blue = blue_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto magenta_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[35m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto magenta = magenta_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto cyan_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[36m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto cyan = cyan_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto white_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[37m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto white = white_v();

//
// Background manipulators
//
template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto on_gray_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[40m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_gray = on_gray_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto on_red_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[41m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_red = on_red_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto on_green_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[42m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_green = on_green_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto on_yellow_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[43m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_yellow = on_yellow_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto on_blue_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[44m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_blue = on_blue_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto on_magenta_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[45m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_magenta = on_magenta_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto on_cyan_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[46m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_cyan = on_cyan_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto on_white_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[47m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto on_white = on_white_v();

//
// Attribute manipulators
//
template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto bold_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[01m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto bold = bold_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto dark_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[02m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto dark = dark_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto underline_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[04m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto underline = underline_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto blink_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[05m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto blink = blink_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto reverse_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[07m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto reverse = reverse_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto concealed_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[08m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto concealed = concealed_v();

template <typename CharT = char>
inline TERMCOLOR2_CXX20_CONSTEVAL_FN auto reset_v() noexcept
    -> std::basic_string<CharT> {
  return CHAR_LITERAL(CharT, "\033[00m");
}
inline TERMCOLOR2_CXX20_CONSTINIT const auto reset = reset_v();

} // end namespace termcolor2

#endif // POAC_UTIL_TERMCOLOR2_PRESETS_HPP_
