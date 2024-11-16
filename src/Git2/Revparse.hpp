#pragma once

#include "Global.hpp"

#include <git2/revparse.h>

namespace git2 {

struct Revspec : public GlobalState {
private:
  git_object* mFromObj = nullptr;
  git_object* mToObj = nullptr;
  unsigned int mModeVal; // git_revparse_mode_t

public:
  /// Assembles a new revspec from the from/to components.
  Revspec(git_object* from, git_object* toO, unsigned int mode)
      : mFromObj(from), mToObj(toO), mModeVal(mode) {}
  Revspec() = delete;
  ~Revspec() = default;

  Revspec(const Revspec&) = delete;
  Revspec(Revspec&&) noexcept = default;
  Revspec& operator=(const Revspec&) = delete;
  Revspec& operator=(Revspec&&) noexcept = default;

  /// Access the `from` range of this revspec.
  git_object* from() const noexcept;

  /// Access the `to` range of this revspec.
  git_object* to() const noexcept;

  /// Returns the intent of the revspec.
  unsigned int mode() const noexcept;
};

} // end namespace git2
