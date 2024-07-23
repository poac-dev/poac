#pragma once

#include <concepts>
#include <optional>
#include <ostream>
#include <type_traits>

inline std::ostream&
operator<<(std::ostream& os, const std::nullopt_t&) {
  os << "None";
  return os;
}

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
concept Writer = std::is_base_of_v<std::ostream, std::remove_reference_t<T>>;

template <typename T>
concept Display = requires(T val, std::ostream& os) {
  { os << val } -> std::convertible_to<std::ostream&>;
};

template <typename T, typename U>
concept Eq = requires(T lhs, U rhs) {
  { lhs == rhs } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept Ne = requires(T lhs, U rhs) {
  { lhs != rhs } -> std::convertible_to<bool>;
};

template <typename T, typename U>
concept Lt = requires(T lhs, U rhs) {
  { lhs < rhs } -> std::convertible_to<bool>;
};
