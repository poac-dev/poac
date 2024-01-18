#pragma once

#include "Rustify/Traits.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

struct PoacError : public std::runtime_error {
  template <typename... Args>
    requires(Display<Args> && ...)
  explicit PoacError(Args&&... args)
      : std::runtime_error(
          (std::ostringstream{} << ... << std::forward<Args>(args)).str()
      ) {}
};
