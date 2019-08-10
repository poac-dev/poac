#ifndef GIT2_REVWALK_HPP
#define GIT2_REVWALK_HPP

#include <string>
#include <git2/revwalk.h>
#include <poac/util/git2-cpp/global.hpp>
#include <poac/util/git2-cpp/exception.hpp>
#include <poac/util/git2-cpp/oid.hpp>
#include <poac/util/git2-cpp/repository.hpp>

namespace git2 {
    struct revwalk {
        git_revwalk* raw = nullptr;

        revwalk() = delete;
        explicit revwalk(const repository& repo) {
            git2_throw(git_revwalk_new(&this->raw, repo.raw));
        }
        ~revwalk() noexcept {
            git_revwalk_free(this->raw);
        }

        revwalk(const revwalk&) = delete;
        revwalk& operator=(const revwalk&) = delete;
        revwalk(revwalk&&) = default;
        revwalk& operator=(revwalk&&) = default;

        /// Reset a revwalk to allow re-configuring it.
        ///
        /// The revwalk is automatically reset when iteration of its commits
        /// completes.
        revwalk& reset() {
            git_revwalk_reset(this->raw);
            return *this;
        }

        /// Set the order in which commits are visited.
        revwalk& set_sorting(unsigned int sort_mode) {
            git_revwalk_sorting(this->raw, sort_mode);
            return *this;
        }

        /// Simplify the history by first-parent
        ///
        /// No parents other than the first for each commit will be enqueued.
        revwalk& simplify_first_parent() {
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
        revwalk& push(const oid& oid_) {
            git2_throw(git_revwalk_push(this->raw, &oid_.raw));
            return *this;
        }

        /// Push the repository's HEAD
        ///
        /// For more information, see `push`.
        revwalk& push_head() {
            git2_throw(git_revwalk_push_head(this->raw));
            return *this;
        }

        /// Push matching references
        ///
        /// The OIDs pointed to by the references that match the given glob pattern
        /// will be pushed to the revision walker.
        ///
        /// A leading 'refs/' is implied if not present as well as a trailing `/ \
        /// *` if the glob lacks '?', ' \ *' or '['.
        ///
        /// Any references matching this glob which do not point to a committish
        /// will be ignored.
        revwalk& push_glob(const std::string& glob) {
            git2_throw(git_revwalk_push_glob(this->raw, glob.c_str()));
            return *this;
        }

        /// Push and hide the respective endpoints of the given range.
        ///
        /// The range should be of the form `<commit>..<commit>` where each
        /// `<commit>` is in the form accepted by `revparse_single`. The left-hand
        /// commit will be hidden and the right-hand commit pushed.
        revwalk& push_range(const std::string& range) {
            git2_throw(git_revwalk_push_range(this->raw, range.c_str()));
            return *this;
        }

        /// Push the OID pointed to by a reference
        ///
        /// The reference must point to a committish.
        revwalk& push_ref(const std::string& reference) {
            git2_throw(git_revwalk_push_ref(this->raw, reference.c_str()));
            return *this;
        }

        /// Mark a commit as not of interest to this revwalk.
        revwalk& hide(const oid& oid_) {
            git2_throw(git_revwalk_hide(this->raw, &oid_.raw));
            return *this;
        }

        /// Hide the repository's HEAD
        ///
        /// For more information, see `hide`.
        revwalk& hide_head() {
            git2_throw(git_revwalk_hide_head(this->raw));
            return *this;
        }

        /// Hide matching references.
        ///
        /// The OIDs pointed to by the references that match the given glob pattern
        /// and their ancestors will be hidden from the output on the revision walk.
        ///
        /// A leading 'refs/' is implied if not present as well as a trailing `/ \
        /// *` if the glob lacks '?', ' \ *' or '['.
        ///
        /// Any references matching this glob which do not point to a committish
        /// will be ignored.
        revwalk& hide_glob(const std::string& glob) {
            git2_throw(git_revwalk_hide_glob(this->raw, glob.c_str()));
            return *this;
        }

        /// Hide the OID pointed to by a reference.
        ///
        /// The reference must point to a committish.
        revwalk& hide_ref(const std::string& reference) {
            git2_throw(git_revwalk_hide_ref(this->raw, reference.c_str()));
            return *this;
        }

        /// Get the next commit from the revision walk.
        oid next() {
            git_oid oid_;
            git2_throw(git_revwalk_next(&oid_, this->raw)); // FIXME
            return oid(oid_);
        }
    };
} // end namespace git2

#endif	// !GIT2_REVWALK_HPP
