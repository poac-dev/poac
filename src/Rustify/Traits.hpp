#pragma once

#include <concepts>
#include <optional>
#include <ostream>

template <typename T>
std::ostream&
operator<<(std::ostream& os, const std::optional<T>& opt) {
  if (opt.has_value()) {
    os << opt.value();
  } else {
    os << "None";
  }
  return os;
}

template <typename T>
concept Printable = requires(T a, std::ostream& os) {
  { os << a } -> std::convertible_to<std::ostream&>;
};

template <typename T, typename U>
concept Eq = requires(T a, U b) {
  { a == b } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept Ne = requires(T a, U b) {
  { a != b } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept Lt = requires(T a, U b) {
  { a < b } -> std::convertible_to<bool>;
};
