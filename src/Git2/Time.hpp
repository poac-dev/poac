#pragma once

#include "../Rustify.hpp"

#include <git2/types.h>
#include <string>

namespace git2 {

struct Time {
  git_time_t mTime;

  std::string toString() const;
};

} // namespace git2
