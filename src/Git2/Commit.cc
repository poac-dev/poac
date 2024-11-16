#include "Commit.hpp"

#include "Exception.hpp"
#include "Oid.hpp"
#include "Repository.hpp"
#include "Time.hpp"

#include <git2/commit.h>

namespace git2 {

Commit::~Commit() {
  git_commit_free(mRaw);
}

Commit&
Commit::lookup(const Repository& repo, const Oid& oid) {
  git2Throw(git_commit_lookup(&mRaw, repo.mRaw, oid.mRaw));
  return *this;
}

Time
Commit::time() const {
  return { git_commit_time(mRaw) };
}

} // namespace git2
