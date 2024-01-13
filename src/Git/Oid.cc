#include "Oid.hpp"

#include "../Rustify.hpp"
#include "Exception.hpp"
#include "Global.hpp"

#include <git2/oid.h>
#include <memory>
#include <ostream>

namespace git2 {

Oid::Oid(const StringRef str) {
  git2::init();
  git2Throw(git_oid_fromstrn(&this->raw, str.data(), str.size()));
}

Oid::Oid(const std::unique_ptr<unsigned char>& bytes) {
  git2::init();
  git_oid_fromraw(&this->raw, bytes.get());
}

/// Test if this OID is all zeros.
bool Oid::isZero() const {
#if (LIBGIT2_VER_MAJOR < 1) && (LIBGIT2_VER_MINOR < 99)
  return git_oid_iszero(&raw) == 1;
#else
  return git_oid_is_zero(&raw) == 1;
#endif
}

std::ostream& operator<<(std::ostream& os, const Oid& o) {
  return (os << git_oid_tostr_s(&o.raw));
}

inline bool operator==(const Oid& lhs, const Oid& rhs) {
  return git_oid_equal(&lhs.raw, &rhs.raw) != 0;
}

} // end namespace git2
