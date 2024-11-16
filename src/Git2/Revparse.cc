#include "Revparse.hpp"

#include <git2/revparse.h>

namespace git2 {

git_object*
Revspec::from() const noexcept {
  return mFromObj;
}

git_object*
Revspec::to() const noexcept {
  return mToObj;
}

unsigned int
Revspec::mode() const noexcept {
  return mModeVal;
}

} // end namespace git2
