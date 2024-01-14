#include "Oid.hpp"

#include "../Rustify.hpp"
#include "Exception.hpp"
#include "Global.hpp"

#include <cstring>
#include <git2/oid.h>
#include <ostream>

namespace git2 {

Oid::Oid(git_oid* oid) : raw(oid) {
  git2::init();
}

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
Oid::Oid(const git_oid* oid) : Oid(const_cast<git_oid*>(oid)) {}

Oid::Oid(const StringRef str) {
  git2::init();
  git2Throw(git_oid_fromstrn(raw, str.data(), str.size()));
}

/// Test if this OID is all zeros.
bool
Oid::isZero() const {
#if (LIBGIT2_VER_MAJOR < 1) && (LIBGIT2_VER_MINOR < 99)
  return git_oid_iszero(raw) == 1;
#else
  return git_oid_is_zero(raw) == 1;
#endif
}

std::ostream&
operator<<(std::ostream& os, const Oid& o) {
  return (os << git_oid_tostr_s(o.raw));
}

inline bool
operator==(const Oid& lhs, const Oid& rhs) {
  return git_oid_equal(lhs.raw, rhs.raw) != 0;
}

} // end namespace git2
