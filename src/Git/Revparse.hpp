#pragma once

#include <git2/revparse.h>

namespace git2 {

struct revspec {
private:
  git_object* from_ = nullptr;
  git_object* to_ = nullptr;
  unsigned int mode_; // git_revparse_mode_t

public:
  /// Assembles a new revspec from the from/to components.
  revspec(git_object* from, git_object* to, unsigned int mode)
      : from_(from), to_(to), mode_(mode) {}
  revspec() = delete;
  ~revspec() = default;

  revspec(const revspec&) = delete;
  revspec& operator=(const revspec&) = delete;
  revspec(revspec&&) = default;
  revspec& operator=(revspec&&) = default;

  /// Access the `from` range of this revspec.
  git_object* from() const noexcept {
    return this->from_;
  }

  /// Access the `to` range of this revspec.
  git_object* to() const noexcept {
    return this->to_;
  }

  /// Returns the intent of the revspec.
  unsigned int mode() const noexcept {
    return this->mode_;
  }
};

} // end namespace git2
