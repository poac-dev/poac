#include "Revwalk.hpp"

#include "Exception.hpp"
#include "Repository.hpp"

#include <git2/revwalk.h>
#include <string>

namespace git2 {

Revwalk::Revwalk(const Repository& repo) {
  git2Throw(git_revwalk_new(&mRaw, repo.mRaw));
}
Revwalk::~Revwalk() noexcept {
  git_revwalk_free(mRaw);
}

Revwalk&
Revwalk::reset() {
  git_revwalk_reset(mRaw);
  return *this;
}

Revwalk&
Revwalk::setSorting(unsigned int sortMode) {
  git_revwalk_sorting(mRaw, sortMode);
  return *this;
}

Revwalk&
Revwalk::simplifyFirstParent() {
  git_revwalk_simplify_first_parent(mRaw);
  return *this;
}

Revwalk&
Revwalk::push(const Oid& oid) {
  git2Throw(git_revwalk_push(mRaw, oid.mRaw));
  return *this;
}

Revwalk&
Revwalk::pushHead() {
  git2Throw(git_revwalk_push_head(mRaw));
  return *this;
}

Revwalk&
Revwalk::pushGlob(const std::string& glob) {
  git2Throw(git_revwalk_push_glob(mRaw, glob.c_str()));
  return *this;
}

Revwalk&
Revwalk::pushRange(const std::string& range) {
  git2Throw(git_revwalk_push_range(mRaw, range.c_str()));
  return *this;
}

Revwalk&
Revwalk::pushRef(const std::string& reference) {
  git2Throw(git_revwalk_push_ref(mRaw, reference.c_str()));
  return *this;
}

Revwalk&
Revwalk::hide(const Oid& oid) {
  git2Throw(git_revwalk_hide(mRaw, oid.mRaw));
  return *this;
}

Revwalk&
Revwalk::hideHead() {
  git2Throw(git_revwalk_hide_head(mRaw));
  return *this;
}

Revwalk&
Revwalk::hideGlob(const std::string& glob) {
  git2Throw(git_revwalk_hide_glob(mRaw, glob.c_str()));
  return *this;
}

Revwalk&
Revwalk::hideRef(const std::string& reference) {
  git2Throw(git_revwalk_hide_ref(mRaw, reference.c_str()));
  return *this;
}

} // end namespace git2
