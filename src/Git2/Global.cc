#include "Global.hpp"

#include "Exception.hpp"

#include <git2/global.h>
#include <mutex>

namespace git2 {
namespace detail {

  static void init() {
    git2Throw(git_libgit2_init());
  }

} // namespace detail

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables)
static inline std::once_flag once;

void
init() {
  std::call_once(once, detail::init);
}

} // namespace git2
