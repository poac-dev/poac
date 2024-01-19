#pragma once

#include "Global.hpp"
#include "Oid.hpp"

#include <git2/types.h>

namespace git2 {

struct Object : public GlobalState {
  git_object* raw = nullptr;

  Object() = default;
  ~Object();

  Object(const Object&) = delete;
  Object& operator=(const Object&) = delete;
  Object(Object&&) = default;
  Object& operator=(Object&&) = default;

  explicit Object(git_object*);

  /// Get the id (SHA1) of a repository object.
  Oid id() const;
};

} // namespace git2
