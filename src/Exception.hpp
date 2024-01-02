#pragma once

#include "Algos.hpp"
#include "Rustify.hpp"

#include <stdexcept>
#include <utility>

struct PoacError : public std::runtime_error {
  template <typename... Args>
  explicit PoacError(Args&&... args)
      : std::runtime_error(concat(std::forward<Args>(args)...)) {}
};
