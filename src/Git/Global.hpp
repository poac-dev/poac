#pragma once

#include "Exception.hpp"

#include <git2/global.h>
#include <mutex>

namespace git2 {
namespace detail {

  inline void init() {
    git2Throw(git_libgit2_init());
  }

} // namespace detail

static inline std::once_flag once;

inline void init() {
  std::call_once(once, detail::init);
}

} // end namespace git2
