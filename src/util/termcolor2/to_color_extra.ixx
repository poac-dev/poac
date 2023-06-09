module;

// std
#include <cstddef> // std::size_t
#include <string> // std::basic_string
#include <string_view> // std::basic_string_view

export module termcolor2.to_color_extra;

import termcolor2.color_mode;
import termcolor2.presets;

export namespace termcolor2 {

template <typename Func, typename CharT>
inline auto to_bold_color(Func&& fn, const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  if (should_color()) {
    return bold_v<CharT>() + fn() + str + reset_v<CharT>();
  } else {
    return str;
  }
}

//
// Foreground manipulators
//
template <typename CharT>
inline auto to_bold_gray(const CharT* str) -> std::basic_string<CharT> {
  return to_bold_color(gray_v<CharT>, std::basic_string<CharT>(str));
}
template <typename CharT>
inline auto to_bold_gray(const CharT* str, std::size_t len)
    -> std::basic_string<CharT> {
  return to_bold_color(gray_v<CharT>, std::basic_string<CharT>(str, len));
}
template <typename CharT>
inline auto to_bold_gray(const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  return to_bold_color(gray_v<CharT>, str);
}
template <typename CharT>
inline auto to_bold_gray(const std::basic_string_view<CharT> str)
    -> std::basic_string<CharT> {
  return to_bold_color(gray_v<CharT>, std::basic_string<CharT>(str));
}

template <typename CharT>
inline auto to_bold_red(const CharT* str) -> std::basic_string<CharT> {
  return to_bold_color(red_v<CharT>, std::basic_string<CharT>(str));
}
template <typename CharT>
inline auto to_bold_red(const CharT* str, std::size_t len)
    -> std::basic_string<CharT> {
  return to_bold_color(red_v<CharT>, std::basic_string<CharT>(str, len));
}
template <typename CharT>
inline auto to_bold_red(const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  return to_bold_color(red_v<CharT>, str);
}
template <typename CharT>
inline auto to_bold_red(const std::basic_string_view<CharT> str)
    -> std::basic_string<CharT> {
  return to_bold_color(red_v<CharT>, std::basic_string<CharT>(str));
}

template <typename CharT>
inline auto to_bold_green(const CharT* str) -> std::basic_string<CharT> {
  return to_bold_color(green_v<CharT>, std::basic_string<CharT>(str));
}
template <typename CharT>
inline auto to_bold_green(const CharT* str, std::size_t len)
    -> std::basic_string<CharT> {
  return to_bold_color(green_v<CharT>, std::basic_string<CharT>(str, len));
}
template <typename CharT>
inline auto to_bold_green(const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  return to_bold_color(green_v<CharT>, str);
}
template <typename CharT>
inline auto to_bold_green(const std::basic_string_view<CharT> str)
    -> std::basic_string<CharT> {
  return to_bold_color(green_v<CharT>, std::basic_string<CharT>(str));
}

template <typename CharT>
inline auto to_bold_yellow(const CharT* str) -> std::basic_string<CharT> {
  return to_bold_color(yellow_v<CharT>, std::basic_string<CharT>(str));
}
template <typename CharT>
inline auto to_bold_yellow(const CharT* str, std::size_t len)
    -> std::basic_string<CharT> {
  return to_bold_color(yellow_v<CharT>, std::basic_string<CharT>(str, len));
}
template <typename CharT>
inline auto to_bold_yellow(const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  return to_bold_color(yellow_v<CharT>, str);
}
template <typename CharT>
inline auto to_bold_yellow(const std::basic_string_view<CharT> str)
    -> std::basic_string<CharT> {
  return to_bold_color(yellow_v<CharT>, std::basic_string<CharT>(str));
}

template <typename CharT>
inline auto to_bold_blue(const CharT* str) -> std::basic_string<CharT> {
  return to_bold_color(blue_v<CharT>, std::basic_string<CharT>(str));
}
template <typename CharT>
inline auto to_bold_blue(const CharT* str, std::size_t len)
    -> std::basic_string<CharT> {
  return to_bold_color(blue_v<CharT>, std::basic_string<CharT>(str, len));
}
template <typename CharT>
inline auto to_bold_blue(const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  return to_bold_color(blue_v<CharT>, str);
}
template <typename CharT>
inline auto to_bold_blue(const std::basic_string_view<CharT> str)
    -> std::basic_string<CharT> {
  return to_bold_color(blue_v<CharT>, std::basic_string<CharT>(str));
}

template <typename CharT>
inline auto to_bold_magenta(const CharT* str) -> std::basic_string<CharT> {
  return to_bold_color(magenta_v<CharT>, std::basic_string<CharT>(str));
}
template <typename CharT>
inline auto to_bold_magenta(const CharT* str, std::size_t len)
    -> std::basic_string<CharT> {
  return to_bold_color(magenta_v<CharT>, std::basic_string<CharT>(str, len));
}
template <typename CharT>
inline auto to_bold_magenta(const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  return to_bold_color(magenta_v<CharT>, str);
}
template <typename CharT>
inline auto to_bold_magenta(const std::basic_string_view<CharT> str)
    -> std::basic_string<CharT> {
  return to_bold_color(magenta_v<CharT>, std::basic_string<CharT>(str));
}

template <typename CharT>
inline auto to_bold_cyan(const CharT* str) -> std::basic_string<CharT> {
  return to_bold_color(cyan_v<CharT>, std::basic_string<CharT>(str));
}
template <typename CharT>
inline auto to_bold_cyan(const CharT* str, std::size_t len)
    -> std::basic_string<CharT> {
  return to_bold_color(cyan_v<CharT>, std::basic_string<CharT>(str, len));
}
template <typename CharT>
inline auto to_bold_cyan(const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  return to_bold_color(cyan_v<CharT>, str);
}
template <typename CharT>
inline auto to_bold_cyan(const std::basic_string_view<CharT> str)
    -> std::basic_string<CharT> {
  return to_bold_color(cyan_v<CharT>, std::basic_string<CharT>(str));
}

template <typename CharT>
inline auto to_bold_white(const CharT* str) -> std::basic_string<CharT> {
  return to_bold_color(white_v<CharT>, std::basic_string<CharT>(str));
}
template <typename CharT>
inline auto to_bold_white(const CharT* str, std::size_t len)
    -> std::basic_string<CharT> {
  return to_bold_color(white_v<CharT>, std::basic_string<CharT>(str, len));
}
template <typename CharT>
inline auto to_bold_white(const std::basic_string<CharT>& str)
    -> std::basic_string<CharT> {
  return to_bold_color(white_v<CharT>, str);
}
template <typename CharT>
inline auto to_bold_white(const std::basic_string_view<CharT> str)
    -> std::basic_string<CharT> {
  return to_bold_color(white_v<CharT>, std::basic_string<CharT>(str));
}

} // namespace termcolor2
