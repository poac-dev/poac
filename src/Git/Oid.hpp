#pragma once

#include "../Rustify.hpp"

#include <git2/oid.h>
#include <memory>
#include <ostream>

namespace git2 {

struct Oid {
  git_oid raw;

  explicit Oid(const git_oid& oid) : raw(oid) {}

  /// Parse a hex-formatted object id into an oid structure.
  explicit Oid(const StringRef);

  /// Parse a hex-formatted object id into an oid structure.
  explicit Oid(const std::unique_ptr<unsigned char>&);

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
