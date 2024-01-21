#include "Revwalk.hpp"

#include "../Rustify.hpp"
#include "Exception.hpp"
#include "Repository.hpp"

#include <git2/revwalk.h>

namespace git2 {

Revwalk::Revwalk(const Repository& repo) {
  git2Throw(git_revwalk_new(&this->raw, repo.raw));
}
Revwalk::~Revwalk() noexcept {
  git_revwalk_free(this->raw);
}

Revwalk&
Revwalk::reset() {
  git_revwalk_reset(this->raw);
  return *this;
}

Revwalk&
Revwalk::setSorting(unsigned int sort_mode) {
  git_revwalk_sorting(this->raw, sort_mode);
  return *this;
}

Revwalk&
Revwalk::simplifyFirstParent() {
  git_revwalk_simplify_first_parent(this->raw);
  return *this;
}

Revwalk&
Revwalk::push(const Oid& oid) {
  git2Throw(git_revwalk_push(this->raw, oid.raw));
  return *this;
}

Revwalk&
Revwalk::pushHead() {
  git2Throw(git_revwalk_push_head(this->raw));
  return *this;
}

Revwalk&
Revwalk::pushGlob(const StringRef glob) {
  git2Throw(git_revwalk_push_glob(this->raw, glob.data()));
  return *this;
}

Revwalk&
Revwalk::pushRange(const StringRef range) {
  git2Throw(git_revwalk_push_range(this->raw, range.data()));
  return *this;
}

Revwalk&
Revwalk::pushRef(const StringRef reference) {
  git2Throw(git_revwalk_push_ref(this->raw, reference.data()));
  return *this;
}

Revwalk&
Revwalk::hide(const Oid& oid) {
  git2Throw(git_revwalk_hide(this->raw, oid.raw));
  return *this;
}

Revwalk&
Revwalk::hideHead() {
  git2Throw(git_revwalk_hide_head(this->raw));
  return *this;
}

Revwalk&
Revwalk::hideGlob(const StringRef glob) {
  git2Throw(git_revwalk_hide_glob(this->raw, glob.data()));
  return *this;
}

Revwalk&
Revwalk::hideRef(const StringRef reference) {
  git2Throw(git_revwalk_hide_ref(this->raw, reference.data()));
  return *this;
}

} // end namespace git2
