#pragma once

#include "../Rustify.hpp"

#include <git2/types.h>

namespace git2 {

struct Time {
  git_time_t time;

  String toString() const;
};

} // namespace git2
