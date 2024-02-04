#pragma once

#include "Rustify/Traits.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

struct PoacError : public std::runtime_error {
  explicit PoacError(Display auto&&... args)
      : std::runtime_error( //
          (std::ostringstream{} << ... << std::forward<decltype(args)>(args))
              .str()
      ) {}
};
