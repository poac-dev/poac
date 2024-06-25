#include "Exception.hpp"

#include "../Rustify.hpp"

#include <git2/errors.h>
#include <git2/version.h>

#if (LIBGIT2_VER_MAJOR >= 1) && (LIBGIT2_VER_MINOR >= 8)
#  include <git2/sys/errors.h>
#endif

namespace git2 {

#if (LIBGIT2_VER_MAJOR < 1) && (LIBGIT2_VER_MINOR < 28)
// NOLINTBEGIN(readability-identifier-naming)
const git_error*
git_error_last() {
  return giterr_last();
}

void
git_error_clear() {
  giterr_clear();
}
// NOLINTEND(readability-identifier-naming)
#endif

Exception::Exception() {
  if (const git_error* error = git_error_last(); error != nullptr) {
    this->msg += error->message;
    this->cat = static_cast<git_error_t>(error->klass);
    git_error_clear();
  }
}

const char*
Exception::what() const noexcept {
  return this->msg.c_str();
}
git_error_t
Exception::category() const noexcept {
  return this->cat;
}

int
git2Throw(const int ret) {
  if (ret < GIT_OK) {
    throw Exception();
  }
  return ret;
}

} // namespace git2
