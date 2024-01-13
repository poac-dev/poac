#include "Revparse.hpp"

#include <git2/revparse.h>

namespace git2 {

git_object* Revspec::from() const noexcept {
  return this->from_;
}

git_object* Revspec::to() const noexcept {
  return this->to_;
}

unsigned int Revspec::mode() const noexcept {
  return this->mode_;
}

} // end namespace git2
