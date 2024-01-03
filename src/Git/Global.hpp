#pragma once

#include "Exception.hpp"

#include <git2/global.h>
#include <mutex>

namespace git2 {
namespace detail {

  void init() {
    git2_throw(git_libgit2_init());
  }

} // namespace detail

static inline std::once_flag once;
void init() {
  std::call_once(once, detail::init);
}

} // end namespace git2
