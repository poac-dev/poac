#pragma once

// std
#include <cstddef> // std::size_t
#include <string> // std::basic_string

// internal
#include "poac/util/termcolor2/to_color_extra.hpp"

namespace termcolor2::inline color_literals::inline foreground_literals {

inline auto operator"" _bold_gray(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold_gray(str, len);
}
inline auto operator"" _bold_gray(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold_gray(str, len);
}
inline auto operator"" _bold_gray(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold_gray(str, len);
}
inline auto operator"" _bold_gray(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold_gray(str, len);
}
inline auto operator"" _bold_gray(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold_gray(str, len);
}

inline auto operator"" _bold_red(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold_red(str, len);
}
inline auto operator"" _bold_red(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold_red(str, len);
}
inline auto operator"" _bold_red(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold_red(str, len);
}
inline auto operator"" _bold_red(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold_red(str, len);
}
inline auto operator"" _bold_red(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold_red(str, len);
}

inline auto operator"" _bold_green(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold_green(str, len);
}
inline auto operator"" _bold_green(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold_green(str, len);
}
inline auto operator"" _bold_green(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold_green(str, len);
}
inline auto operator"" _bold_green(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold_green(str, len);
}
inline auto operator"" _bold_green(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold_green(str, len);
}

inline auto operator"" _bold_yellow(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold_yellow(str, len);
}
inline auto operator"" _bold_yellow(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold_yellow(str, len);
}
inline auto operator"" _bold_yellow(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold_yellow(str, len);
}
inline auto operator"" _bold_yellow(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold_yellow(str, len);
}
inline auto operator"" _bold_yellow(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold_yellow(str, len);
}

inline auto operator"" _bold_blue(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold_blue(str, len);
}
inline auto operator"" _bold_blue(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold_blue(str, len);
}
inline auto operator"" _bold_blue(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold_blue(str, len);
}
inline auto operator"" _bold_blue(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold_blue(str, len);
}
inline auto operator"" _bold_blue(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold_blue(str, len);
}

inline auto operator"" _bold_magenta(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold_magenta(str, len);
}
inline auto operator"" _bold_magenta(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold_magenta(str, len);
}
inline auto operator"" _bold_magenta(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold_magenta(str, len);
}
inline auto operator"" _bold_magenta(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold_magenta(str, len);
}
inline auto operator"" _bold_magenta(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold_magenta(str, len);
}

inline auto operator"" _bold_cyan(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold_cyan(str, len);
}
inline auto operator"" _bold_cyan(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold_cyan(str, len);
}
inline auto operator"" _bold_cyan(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold_cyan(str, len);
}
inline auto operator"" _bold_cyan(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold_cyan(str, len);
}
inline auto operator"" _bold_cyan(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold_cyan(str, len);
}

inline auto operator"" _bold_white(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold_white(str, len);
}
inline auto operator"" _bold_white(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold_white(str, len);
}
inline auto operator"" _bold_white(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold_white(str, len);
}
inline auto operator"" _bold_white(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold_white(str, len);
}
inline auto operator"" _bold_white(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold_white(str, len);
}

// clang-format off
// to avoid reporting errors with inline namespace on only the dry-run mode. (IDK why)
}  // namespace termcolor2::color_literals::foreground_literals
// clang-format on
