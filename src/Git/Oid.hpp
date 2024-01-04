#pragma once

#include "Exception.hpp"
#include "Global.hpp"

#include <git2/oid.h>
#include <iostream>
#include <memory>
#include <string>

namespace git2 {

struct Oid {
  git_oid raw;

  explicit Oid(const git_oid& raw) : raw(raw) {}

  /// Parse a hex-formatted object id into an oid structure.
  explicit Oid(const std::string& str) {
    git2::init();
    git2Throw(git_oid_fromstrn(&this->raw, str.c_str(), str.size()));
  }

  /// Parse a hex-formatted object id into an oid structure.
  explicit Oid(const std::unique_ptr<unsigned char>& bytes) {
    git2::init();
    git_oid_fromraw(&this->raw, bytes.get());
  }

  Oid() = delete;
  ~Oid() = default;

  Oid(const Oid&) = default;
  Oid& operator=(const Oid&) = default;
  Oid(Oid&&) = default;
  Oid& operator=(Oid&&) = default;

  /// Test if this OID is all zeros.
  bool isZero() const {
#if (LIBGIT2_VER_MAJOR < 1) && (LIBGIT2_VER_MINOR < 99)
    return git_oid_iszero(&raw) == 1;
#else
    return git_oid_is_zero(&raw) == 1;
#endif
  }

  friend std::ostream& operator<<(std::ostream& os, const Oid& o) {
    return (os << git_oid_tostr_s(&o.raw));
  }
};

inline bool operator==(const Oid& lhs, const Oid& rhs) {
  return git_oid_equal(&lhs.raw, &rhs.raw) != 0;
}

} // end namespace git2
