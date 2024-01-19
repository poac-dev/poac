#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <git2/oid.h>
#include <ostream>

namespace git2 {

struct Oid : public GlobalState {
  git_oid* raw = nullptr;

  explicit Oid(git_oid*);

  explicit Oid(const git_oid*);

  /// Parse a hex-formatted object id into an oid structure.
  explicit Oid(StringRef);

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
};

std::ostream& operator<<(std::ostream&, const Oid&);
bool operator==(const Oid&, const Oid&);

} // end namespace git2
