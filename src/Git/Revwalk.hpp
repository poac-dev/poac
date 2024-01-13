#pragma once

#include "../Rustify.hpp"
#include "Exception.hpp"
#include "Global.hpp"
#include "Oid.hpp"
#include "Repository.hpp"

#include <git2/revwalk.h>
#include <string>

namespace git2 {

struct Revwalk {
  git_revwalk* raw = nullptr;

  Revwalk() = delete;
  explicit Revwalk(const Repository& repo) {
    git2Throw(git_revwalk_new(&this->raw, repo.raw));
  }
  ~Revwalk() noexcept {
    git_revwalk_free(this->raw);
  }

  Revwalk(const Revwalk&) = delete;
  Revwalk& operator=(const Revwalk&) = delete;
  Revwalk(Revwalk&&) = default;
  Revwalk& operator=(Revwalk&&) = default;

  /// Reset a revwalk to allow re-configuring it.
  ///
  /// The revwalk is automatically reset when iteration of its commits
  /// completes.
  Revwalk& reset() {
    git_revwalk_reset(this->raw);
    return *this;
  }

  /// Set the order in which commits are visited.
  Revwalk& setSorting(unsigned int sort_mode) {
    git_revwalk_sorting(this->raw, sort_mode);
    return *this;
  }

  /// Simplify the history by first-parent
  ///
  /// No parents other than the first for each commit will be enqueued.
  Revwalk& simplifyFirstParent() {
    git_revwalk_simplify_first_parent(this->raw);
    return *this;
  }

  /// Mark a commit to start traversal from.
  ///
  /// The given OID must belong to a committish on the walked repository.
  ///
  /// The given commit will be used as one of the roots when starting the
  /// revision walk. At least one commit must be pushed onto the walker before
  /// a walk can be started.
  Revwalk& push(const Oid& oid_) {
    git2Throw(git_revwalk_push(this->raw, &oid_.raw));
    return *this;
  }

  /// Push the repository's HEAD
  ///
  /// For more information, see `push`.
  Revwalk& pushHead() {
    git2Throw(git_revwalk_push_head(this->raw));
    return *this;
  }

  /// Push matching references
  ///
  /// The OIDs pointed to by the references that match the given glob pattern
  /// will be pushed to the revision walker.
  ///
  /// A leading 'refs/' is implied if not present as well as a trailing `/ \ *`
  /// if the glob lacks '?', ' \ *' or '['.
  ///
  /// Any references matching this glob which do not point to a committish
  /// will be ignored.
  Revwalk& pushGlob(const StringRef glob) {
    git2Throw(git_revwalk_push_glob(this->raw, glob.data()));
    return *this;
  }

  /// Push and hide the respective endpoints of the given range.
  ///
  /// The range should be of the form `<commit>..<commit>` where each
  /// `<commit>` is in the form accepted by `revparse_single`. The left-hand
  /// commit will be hidden and the right-hand commit pushed.
  Revwalk& pushRange(const StringRef range) {
    git2Throw(git_revwalk_push_range(this->raw, range.data()));
    return *this;
  }

  /// Push the OID pointed to by a reference
  ///
  /// The reference must point to a committish.
  Revwalk& pushRef(const StringRef reference) {
    git2Throw(git_revwalk_push_ref(this->raw, reference.data()));
    return *this;
  }

  /// Mark a commit as not of interest to this revwalk.
  Revwalk& hide(const Oid& oid) {
    git2Throw(git_revwalk_hide(this->raw, &oid.raw));
    return *this;
  }

  /// Hide the repository's HEAD
  ///
  /// For more information, see `hide`.
  Revwalk& hideHead() {
    git2Throw(git_revwalk_hide_head(this->raw));
    return *this;
  }

  /// Hide matching references.
  ///
  /// The OIDs pointed to by the references that match the given glob pattern
  /// and their ancestors will be hidden from the output on the revision walk.
  ///
  /// A leading 'refs/' is implied if not present as well as a trailing `/ \ *`
  /// if the glob lacks '?', ' \ *' or '['.
  ///
  /// Any references matching this glob which do not point to a committish
  /// will be ignored.
  Revwalk& hideGlob(const StringRef glob) {
    git2Throw(git_revwalk_hide_glob(this->raw, glob.data()));
    return *this;
  }

  /// Hide the OID pointed to by a reference.
  ///
  /// The reference must point to a committish.
  Revwalk& hideRef(const StringRef reference) {
    git2Throw(git_revwalk_hide_ref(this->raw, reference.data()));
    return *this;
  }

  /// Get the next commit from the revision walk.
  Oid next() const {
    git_oid oid;
    git2Throw(git_revwalk_next(&oid, this->raw)); // FIXME
    return Oid(oid);
  }
};

} // end namespace git2
