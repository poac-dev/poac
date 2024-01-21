#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <git2/oid.h>
#include <ostream>

namespace git2 {

struct Oid : public GlobalState {
  // TODO: ideally, use one of the following:
  git_oid oid;
  git_oid* raw = nullptr;

  explicit Oid(git_oid oid);

  explicit Oid(git_oid* oid);

  explicit Oid(const git_oid* oid);

  /// Parse a hex-formatted object id into an oid structure.
  explicit Oid(StringRef str);

  // Since Oid would not be constructed by itself, the destructor is not
  // responsible for freeing the raw pointer.
  Oid() = delete;
  ~Oid() = default;

  Oid(const Oid&) = default;
  Oid& operator=(const Oid&) = default;
  Oid(Oid&&) = default;
  Oid& operator=(Oid&&) = default;

  /// Test if this OID is all zeros.
  bool isZero() const;

  /// Format a git_oid into a buffer as a hex string.
  String toString() const;
};

std::ostream& operator<<(std::ostream& os, const Oid& o);
bool operator==(const Oid& lhs, const Oid& rhs);

} // end namespace git2
