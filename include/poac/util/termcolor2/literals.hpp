#pragma once

// std
#include <cstddef> // std::size_t
#include <string> // std::basic_string

// internal
#include "poac/util/termcolor2/to_color.hpp"

namespace termcolor2::inline color_literals::inline foreground_literals {

inline auto operator"" _gray(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_gray(str, len);
}
inline auto operator"" _gray(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_gray(str, len);
}
inline auto operator"" _gray(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_gray(str, len);
}
inline auto operator"" _gray(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_gray(str, len);
}
inline auto operator"" _gray(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_gray(str, len);
}

inline auto operator"" _red(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_red(str, len);
}
inline auto operator"" _red(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_red(str, len);
}
inline auto operator"" _red(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_red(str, len);
}
inline auto operator"" _red(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_red(str, len);
}
inline auto operator"" _red(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_red(str, len);
}

inline auto operator"" _green(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_green(str, len);
}
inline auto operator"" _green(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_green(str, len);
}
inline auto operator"" _green(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_green(str, len);
}
inline auto operator"" _green(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_green(str, len);
}
inline auto operator"" _green(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_green(str, len);
}

inline auto operator"" _yellow(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_yellow(str, len);
}
inline auto operator"" _yellow(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_yellow(str, len);
}
inline auto operator"" _yellow(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_yellow(str, len);
}
inline auto operator"" _yellow(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_yellow(str, len);
}
inline auto operator"" _yellow(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_yellow(str, len);
}

inline auto operator"" _blue(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_blue(str, len);
}
inline auto operator"" _blue(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_blue(str, len);
}
inline auto operator"" _blue(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_blue(str, len);
}
inline auto operator"" _blue(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_blue(str, len);
}
inline auto operator"" _blue(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_blue(str, len);
}

inline auto operator"" _magenta(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_magenta(str, len);
}
inline auto operator"" _magenta(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_magenta(str, len);
}
inline auto operator"" _magenta(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_magenta(str, len);
}
inline auto operator"" _magenta(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_magenta(str, len);
}
inline auto operator"" _magenta(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_magenta(str, len);
}

inline auto operator"" _cyan(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_cyan(str, len);
}
inline auto operator"" _cyan(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_cyan(str, len);
}
inline auto operator"" _cyan(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_cyan(str, len);
}
inline auto operator"" _cyan(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_cyan(str, len);
}
inline auto operator"" _cyan(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_cyan(str, len);
}

inline auto operator"" _white(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_white(str, len);
}
inline auto operator"" _white(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_white(str, len);
}
inline auto operator"" _white(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_white(str, len);
}
inline auto operator"" _white(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_white(str, len);
}
inline auto operator"" _white(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_white(str, len);
}

// clang-format off
// to avoid reporting errors with inline namespace on only the dry-run mode. (IDK why)
}  // namespace termcolor2::color_literals::foreground_literals
// clang-format on

namespace termcolor2::inline color_literals::inline background_literals {
inline auto operator"" _on_gray(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_on_gray(str, len);
}
inline auto operator"" _on_gray(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_on_gray(str, len);
}
inline auto operator"" _on_gray(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_on_gray(str, len);
}
inline auto operator"" _on_gray(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_on_gray(str, len);
}
inline auto operator"" _on_gray(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_on_gray(str, len);
}

inline auto operator"" _on_red(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_on_red(str, len);
}
inline auto operator"" _on_red(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_on_red(str, len);
}
inline auto operator"" _on_red(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_on_red(str, len);
}
inline auto operator"" _on_red(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_on_red(str, len);
}
inline auto operator"" _on_red(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_on_red(str, len);
}

inline auto operator"" _on_green(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_on_green(str, len);
}
inline auto operator"" _on_green(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_on_green(str, len);
}
inline auto operator"" _on_green(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_on_green(str, len);
}
inline auto operator"" _on_green(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_on_green(str, len);
}
inline auto operator"" _on_green(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_on_green(str, len);
}

inline auto operator"" _on_yellow(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_on_yellow(str, len);
}
inline auto operator"" _on_yellow(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_on_yellow(str, len);
}
inline auto operator"" _on_yellow(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_on_yellow(str, len);
}
inline auto operator"" _on_yellow(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_on_yellow(str, len);
}
inline auto operator"" _on_yellow(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_on_yellow(str, len);
}

inline auto operator"" _on_blue(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_on_blue(str, len);
}
inline auto operator"" _on_blue(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_on_blue(str, len);
}
inline auto operator"" _on_blue(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_on_blue(str, len);
}
inline auto operator"" _on_blue(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_on_blue(str, len);
}
inline auto operator"" _on_blue(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_on_blue(str, len);
}

inline auto operator"" _on_magenta(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_on_magenta(str, len);
}
inline auto operator"" _on_magenta(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_on_magenta(str, len);
}
inline auto operator"" _on_magenta(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_on_magenta(str, len);
}
inline auto operator"" _on_magenta(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_on_magenta(str, len);
}
inline auto operator"" _on_magenta(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_on_magenta(str, len);
}

inline auto operator"" _on_cyan(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_on_cyan(str, len);
}
inline auto operator"" _on_cyan(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_on_cyan(str, len);
}
inline auto operator"" _on_cyan(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_on_cyan(str, len);
}
inline auto operator"" _on_cyan(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_on_cyan(str, len);
}
inline auto operator"" _on_cyan(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_on_cyan(str, len);
}

inline auto operator"" _on_white(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_on_white(str, len);
}
inline auto operator"" _on_white(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_on_white(str, len);
}
inline auto operator"" _on_white(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_on_white(str, len);
}
inline auto operator"" _on_white(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_on_white(str, len);
}
inline auto operator"" _on_white(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_on_white(str, len);
}

// clang-format off
// to avoid reporting errors with inline namespace on only the dry-run mode. (IDK why)
}  // namespace termcolor2::color_literals::background_literals
// clang-format on

namespace termcolor2::inline color_literals::inline attribute_literals {
inline auto operator"" _bold(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_bold(str, len);
}
inline auto operator"" _bold(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_bold(str, len);
}
inline auto operator"" _bold(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_bold(str, len);
}
inline auto operator"" _bold(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_bold(str, len);
}
inline auto operator"" _bold(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_bold(str, len);
}

inline auto operator"" _dark(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_dark(str, len);
}
inline auto operator"" _dark(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_dark(str, len);
}
inline auto operator"" _dark(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_dark(str, len);
}
inline auto operator"" _dark(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_dark(str, len);
}
inline auto operator"" _dark(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_dark(str, len);
}

inline auto operator"" _underline(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_underline(str, len);
}
inline auto operator"" _underline(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_underline(str, len);
}
inline auto operator"" _underline(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_underline(str, len);
}
inline auto operator"" _underline(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_underline(str, len);
}
inline auto operator"" _underline(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_underline(str, len);
}

inline auto operator"" _blink(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_blink(str, len);
}
inline auto operator"" _blink(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_blink(str, len);
}
inline auto operator"" _blink(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_blink(str, len);
}
inline auto operator"" _blink(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_blink(str, len);
}
inline auto operator"" _blink(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_blink(str, len);
}

inline auto operator"" _reverse(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_reverse(str, len);
}
inline auto operator"" _reverse(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_reverse(str, len);
}
inline auto operator"" _reverse(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_reverse(str, len);
}
inline auto operator"" _reverse(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_reverse(str, len);
}
inline auto operator"" _reverse(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_reverse(str, len);
}

inline auto operator"" _concealed(const char* str, std::size_t len)
    -> std::basic_string<char> {
  return to_concealed(str, len);
}
inline auto operator"" _concealed(const wchar_t* str, std::size_t len)
    -> std::basic_string<wchar_t> {
  return to_concealed(str, len);
}
inline auto operator"" _concealed(const char8_t* str, std::size_t len)
    -> std::basic_string<char8_t> {
  return to_concealed(str, len);
}
inline auto operator"" _concealed(const char16_t* str, std::size_t len)
    -> std::basic_string<char16_t> {
  return to_concealed(str, len);
}
inline auto operator"" _concealed(const char32_t* str, std::size_t len)
    -> std::basic_string<char32_t> {
  return to_concealed(str, len);
}

// clang-format off
// to avoid reporting errors with inline namespace on only the dry-run mode. (IDK why)
}  // namespace termcolor2::color_literals::attribute_literals
// clang-format on

namespace termcolor2::inline control_literals {
// clang-format off
// to avoid reporting errors with inline namespace on only the dry-run mode. (IDK why)
} // namespace termcolor2::inline control_literals
// clang-format on
