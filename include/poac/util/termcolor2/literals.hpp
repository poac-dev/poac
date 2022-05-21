#ifndef TERMCOLOR2_LITERALS_HPP
#define TERMCOLOR2_LITERALS_HPP

// std
#include <cstddef> // std::size_t
#include <string>  // std::basic_string

// internal
#include <poac/util/termcolor2/to_color.hpp>

namespace termcolor2::inline color_literals::inline foreground_literals {

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, gray_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _gray() {
  return to_gray<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _gray(const char* str, std::size_t len) {
  return to_gray(str, len);
}
inline std::basic_string<wchar_t>
operator"" _gray(const wchar_t* str, std::size_t len) {
  return to_gray(str, len);
}
inline std::basic_string<char8_t>
operator"" _gray(const char8_t* str, std::size_t len) {
  return to_gray(str, len);
}
inline std::basic_string<char16_t>
operator"" _gray(const char16_t* str, std::size_t len) {
  return to_gray(str, len);
}
inline std::basic_string<char32_t>
operator"" _gray(const char32_t* str, std::size_t len) {
  return to_gray(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, red_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _red() {
  return to_red<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _red(const char* str, std::size_t len) {
  return to_red(str, len);
}
inline std::basic_string<wchar_t>
operator"" _red(const wchar_t* str, std::size_t len) {
  return to_red(str, len);
}
inline std::basic_string<char8_t>
operator"" _red(const char8_t* str, std::size_t len) {
  return to_red(str, len);
}
inline std::basic_string<char16_t>
operator"" _red(const char16_t* str, std::size_t len) {
  return to_red(str, len);
}
inline std::basic_string<char32_t>
operator"" _red(const char32_t* str, std::size_t len) {
  return to_red(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, green_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _green() {
  return to_green<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _green(const char* str, std::size_t len) {
  return to_green(str, len);
}
inline std::basic_string<wchar_t>
operator"" _green(const wchar_t* str, std::size_t len) {
  return to_green(str, len);
}
inline std::basic_string<char8_t>
operator"" _green(const char8_t* str, std::size_t len) {
  return to_green(str, len);
}
inline std::basic_string<char16_t>
operator"" _green(const char16_t* str, std::size_t len) {
  return to_green(str, len);
}
inline std::basic_string<char32_t>
operator"" _green(const char32_t* str, std::size_t len) {
  return to_green(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, yellow_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _yellow() {
  return to_yellow<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _yellow(const char* str, std::size_t len) {
  return to_yellow(str, len);
}
inline std::basic_string<wchar_t>
operator"" _yellow(const wchar_t* str, std::size_t len) {
  return to_yellow(str, len);
}
inline std::basic_string<char8_t>
operator"" _yellow(const char8_t* str, std::size_t len) {
  return to_yellow(str, len);
}
inline std::basic_string<char16_t>
operator"" _yellow(const char16_t* str, std::size_t len) {
  return to_yellow(str, len);
}
inline std::basic_string<char32_t>
operator"" _yellow(const char32_t* str, std::size_t len) {
  return to_yellow(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, blue_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _blue() {
  return to_blue<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _blue(const char* str, std::size_t len) {
  return to_blue(str, len);
}
inline std::basic_string<wchar_t>
operator"" _blue(const wchar_t* str, std::size_t len) {
  return to_blue(str, len);
}
inline std::basic_string<char8_t>
operator"" _blue(const char8_t* str, std::size_t len) {
  return to_blue(str, len);
}
inline std::basic_string<char16_t>
operator"" _blue(const char16_t* str, std::size_t len) {
  return to_blue(str, len);
}
inline std::basic_string<char32_t>
operator"" _blue(const char32_t* str, std::size_t len) {
  return to_blue(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, magenta_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _magenta() {
  return to_magenta<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _magenta(const char* str, std::size_t len) {
  return to_magenta(str, len);
}
inline std::basic_string<wchar_t>
operator"" _magenta(const wchar_t* str, std::size_t len) {
  return to_magenta(str, len);
}
inline std::basic_string<char8_t>
operator"" _magenta(const char8_t* str, std::size_t len) {
  return to_magenta(str, len);
}
inline std::basic_string<char16_t>
operator"" _magenta(const char16_t* str, std::size_t len) {
  return to_magenta(str, len);
}
inline std::basic_string<char32_t>
operator"" _magenta(const char32_t* str, std::size_t len) {
  return to_magenta(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, cyan_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _cyan() {
  return to_cyan<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _cyan(const char* str, std::size_t len) {
  return to_cyan(str, len);
}
inline std::basic_string<wchar_t>
operator"" _cyan(const wchar_t* str, std::size_t len) {
  return to_cyan(str, len);
}
inline std::basic_string<char8_t>
operator"" _cyan(const char8_t* str, std::size_t len) {
  return to_cyan(str, len);
}
inline std::basic_string<char16_t>
operator"" _cyan(const char16_t* str, std::size_t len) {
  return to_cyan(str, len);
}
inline std::basic_string<char32_t>
operator"" _cyan(const char32_t* str, std::size_t len) {
  return to_cyan(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, white_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _white() {
  return to_white<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _white(const char* str, std::size_t len) {
  return to_white(str, len);
}
inline std::basic_string<wchar_t>
operator"" _white(const wchar_t* str, std::size_t len) {
  return to_white(str, len);
}
inline std::basic_string<char8_t>
operator"" _white(const char8_t* str, std::size_t len) {
  return to_white(str, len);
}
inline std::basic_string<char16_t>
operator"" _white(const char16_t* str, std::size_t len) {
  return to_white(str, len);
}
inline std::basic_string<char32_t>
operator"" _white(const char32_t* str, std::size_t len) {
  return to_white(str, len);
}
#endif
} // namespace termcolor2::inline color_literals::inline foreground_literals

namespace termcolor2::inline color_literals::inline background_literals {
#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, on_gray_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _on_gray() {
  return to_on_gray<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _on_gray(const char* str, std::size_t len) {
  return to_on_gray(str, len);
}
inline std::basic_string<wchar_t>
operator"" _on_gray(const wchar_t* str, std::size_t len) {
  return to_on_gray(str, len);
}
inline std::basic_string<char8_t>
operator"" _on_gray(const char8_t* str, std::size_t len) {
  return to_on_gray(str, len);
}
inline std::basic_string<char16_t>
operator"" _on_gray(const char16_t* str, std::size_t len) {
  return to_on_gray(str, len);
}
inline std::basic_string<char32_t>
operator"" _on_gray(const char32_t* str, std::size_t len) {
  return to_on_gray(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, on_red_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _on_red() {
  return to_on_red<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _on_red(const char* str, std::size_t len) {
  return to_on_red(str, len);
}
inline std::basic_string<wchar_t>
operator"" _on_red(const wchar_t* str, std::size_t len) {
  return to_on_red(str, len);
}
inline std::basic_string<char8_t>
operator"" _on_red(const char8_t* str, std::size_t len) {
  return to_on_red(str, len);
}
inline std::basic_string<char16_t>
operator"" _on_red(const char16_t* str, std::size_t len) {
  return to_on_red(str, len);
}
inline std::basic_string<char32_t>
operator"" _on_red(const char32_t* str, std::size_t len) {
  return to_on_red(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT,
    on_green_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _on_green() {
  return to_on_green<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _on_green(const char* str, std::size_t len) {
  return to_on_green(str, len);
}
inline std::basic_string<wchar_t>
operator"" _on_green(const wchar_t* str, std::size_t len) {
  return to_on_green(str, len);
}
inline std::basic_string<char8_t>
operator"" _on_green(const char8_t* str, std::size_t len) {
  return to_on_green(str, len);
}
inline std::basic_string<char16_t>
operator"" _on_green(const char16_t* str, std::size_t len) {
  return to_on_green(str, len);
}
inline std::basic_string<char32_t>
operator"" _on_green(const char32_t* str, std::size_t len) {
  return to_on_green(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT,
    on_yellow_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _on_yellow() {
  return to_on_yellow<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _on_yellow(const char* str, std::size_t len) {
  return to_on_yellow(str, len);
}
inline std::basic_string<wchar_t>
operator"" _on_yellow(const wchar_t* str, std::size_t len) {
  return to_on_yellow(str, len);
}
inline std::basic_string<char8_t>
operator"" _on_yellow(const char8_t* str, std::size_t len) {
  return to_on_yellow(str, len);
}
inline std::basic_string<char16_t>
operator"" _on_yellow(const char16_t* str, std::size_t len) {
  return to_on_yellow(str, len);
}
inline std::basic_string<char32_t>
operator"" _on_yellow(const char32_t* str, std::size_t len) {
  return to_on_yellow(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, on_blue_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _on_blue() {
  return to_on_blue<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _on_blue(const char* str, std::size_t len) {
  return to_on_blue(str, len);
}
inline std::basic_string<wchar_t>
operator"" _on_blue(const wchar_t* str, std::size_t len) {
  return to_on_blue(str, len);
}
inline std::basic_string<char8_t>
operator"" _on_blue(const char8_t* str, std::size_t len) {
  return to_on_blue(str, len);
}
inline std::basic_string<char16_t>
operator"" _on_blue(const char16_t* str, std::size_t len) {
  return to_on_blue(str, len);
}
inline std::basic_string<char32_t>
operator"" _on_blue(const char32_t* str, std::size_t len) {
  return to_on_blue(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT,
    on_magenta_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _on_magenta() {
  return to_on_magenta<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _on_magenta(const char* str, std::size_t len) {
  return to_on_magenta(str, len);
}
inline std::basic_string<wchar_t>
operator"" _on_magenta(const wchar_t* str, std::size_t len) {
  return to_on_magenta(str, len);
}
inline std::basic_string<char8_t>
operator"" _on_magenta(const char8_t* str, std::size_t len) {
  return to_on_magenta(str, len);
}
inline std::basic_string<char16_t>
operator"" _on_magenta(const char16_t* str, std::size_t len) {
  return to_on_magenta(str, len);
}
inline std::basic_string<char32_t>
operator"" _on_magenta(const char32_t* str, std::size_t len) {
  return to_on_magenta(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, on_cyan_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _on_cyan() {
  return to_on_cyan<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _on_cyan(const char* str, std::size_t len) {
  return to_on_cyan(str, len);
}
inline std::basic_string<wchar_t>
operator"" _on_cyan(const wchar_t* str, std::size_t len) {
  return to_on_cyan(str, len);
}
inline std::basic_string<char8_t>
operator"" _on_cyan(const char8_t* str, std::size_t len) {
  return to_on_cyan(str, len);
}
inline std::basic_string<char16_t>
operator"" _on_cyan(const char16_t* str, std::size_t len) {
  return to_on_cyan(str, len);
}
inline std::basic_string<char32_t>
operator"" _on_cyan(const char32_t* str, std::size_t len) {
  return to_on_cyan(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT,
    on_white_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _on_white() {
  return to_on_white<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _on_white(const char* str, std::size_t len) {
  return to_on_white(str, len);
}
inline std::basic_string<wchar_t>
operator"" _on_white(const wchar_t* str, std::size_t len) {
  return to_on_white(str, len);
}
inline std::basic_string<char8_t>
operator"" _on_white(const char8_t* str, std::size_t len) {
  return to_on_white(str, len);
}
inline std::basic_string<char16_t>
operator"" _on_white(const char16_t* str, std::size_t len) {
  return to_on_white(str, len);
}
inline std::basic_string<char32_t>
operator"" _on_white(const char32_t* str, std::size_t len) {
  return to_on_white(str, len);
}
#endif
} // namespace termcolor2::inline color_literals::inline background_literals

namespace termcolor2::inline color_literals::inline attribute_literals {
#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, bold_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _bold() {
  return to_bold<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _bold(const char* str, std::size_t len) {
  return to_bold(str, len);
}
inline std::basic_string<wchar_t>
operator"" _bold(const wchar_t* str, std::size_t len) {
  return to_bold(str, len);
}
inline std::basic_string<char8_t>
operator"" _bold(const char8_t* str, std::size_t len) {
  return to_bold(str, len);
}
inline std::basic_string<char16_t>
operator"" _bold(const char16_t* str, std::size_t len) {
  return to_bold(str, len);
}
inline std::basic_string<char32_t>
operator"" _bold(const char32_t* str, std::size_t len) {
  return to_bold(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, dark_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _dark() {
  return to_dark<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _dark(const char* str, std::size_t len) {
  return to_dark(str, len);
}
inline std::basic_string<wchar_t>
operator"" _dark(const wchar_t* str, std::size_t len) {
  return to_dark(str, len);
}
inline std::basic_string<char8_t>
operator"" _dark(const char8_t* str, std::size_t len) {
  return to_dark(str, len);
}
inline std::basic_string<char16_t>
operator"" _dark(const char16_t* str, std::size_t len) {
  return to_dark(str, len);
}
inline std::basic_string<char32_t>
operator"" _dark(const char32_t* str, std::size_t len) {
  return to_dark(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT,
    underline_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _underline() {
  return to_underline<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _underline(const char* str, std::size_t len) {
  return to_underline(str, len);
}
inline std::basic_string<wchar_t>
operator"" _underline(const wchar_t* str, std::size_t len) {
  return to_underline(str, len);
}
inline std::basic_string<char8_t>
operator"" _underline(const char8_t* str, std::size_t len) {
  return to_underline(str, len);
}
inline std::basic_string<char16_t>
operator"" _underline(const char16_t* str, std::size_t len) {
  return to_underline(str, len);
}
inline std::basic_string<char32_t>
operator"" _underline(const char32_t* str, std::size_t len) {
  return to_underline(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, blink_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _blink() {
  return to_blink<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _blink(const char* str, std::size_t len) {
  return to_blink(str, len);
}
inline std::basic_string<wchar_t>
operator"" _blink(const wchar_t* str, std::size_t len) {
  return to_blink(str, len);
}
inline std::basic_string<char8_t>
operator"" _blink(const char8_t* str, std::size_t len) {
  return to_blink(str, len);
}
inline std::basic_string<char16_t>
operator"" _blink(const char16_t* str, std::size_t len) {
  return to_blink(str, len);
}
inline std::basic_string<char32_t>
operator"" _blink(const char32_t* str, std::size_t len) {
  return to_blink(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT, reverse_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _reverse() {
  return to_reverse<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _reverse(const char* str, std::size_t len) {
  return to_reverse(str, len);
}
inline std::basic_string<wchar_t>
operator"" _reverse(const wchar_t* str, std::size_t len) {
  return to_reverse(str, len);
}
inline std::basic_string<char8_t>
operator"" _reverse(const char8_t* str, std::size_t len) {
  return to_reverse(str, len);
}
inline std::basic_string<char16_t>
operator"" _reverse(const char16_t* str, std::size_t len) {
  return to_reverse(str, len);
}
inline std::basic_string<char32_t>
operator"" _reverse(const char32_t* str, std::size_t len) {
  return to_reverse(str, len);
}
#endif

#ifdef TERMCOLOR2_USE_GNU_STRING_LITERAL_OPERATOR_TEMPLATE
template <typename CharT, CharT... Str>
constexpr basic_string<
    CharT,
    concealed_v<CharT>().size() + sizeof...(Str) + reset_v<CharT>().size()>
operator"" _concealed() {
  return to_concealed<CharT, sizeof...(Str), Str...>();
}
#else
inline std::basic_string<char>
operator"" _concealed(const char* str, std::size_t len) {
  return to_concealed(str, len);
}
inline std::basic_string<wchar_t>
operator"" _concealed(const wchar_t* str, std::size_t len) {
  return to_concealed(str, len);
}
inline std::basic_string<char8_t>
operator"" _concealed(const char8_t* str, std::size_t len) {
  return to_concealed(str, len);
}
inline std::basic_string<char16_t>
operator"" _concealed(const char16_t* str, std::size_t len) {
  return to_concealed(str, len);
}
inline std::basic_string<char32_t>
operator"" _concealed(const char32_t* str, std::size_t len) {
  return to_concealed(str, len);
}
#endif

} // namespace termcolor2::inline color_literals::inline attribute_literals

namespace termcolor2::inline control_literals {
} // namespace termcolor2::inline control_literals

#endif // !TERMCOLOR2_LITERALS_HPP
