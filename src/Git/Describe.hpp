#pragma once

#include "../Rustify.hpp"
#include "Exception.hpp"
#include "Global.hpp"
#include "Repository.hpp"

#include <git2/describe.h>
#include <string>

namespace git2 {

struct DescribeOptions {
  git_describe_options raw;

  DescribeOptions() {
#if (LIBGIT2_VER_MAJOR < 1) && (LIBGIT2_VER_MINOR < 99)
    git2Throw(
        git_describe_init_options(&this->raw, GIT_DESCRIBE_OPTIONS_VERSION)
    );
#else
    git2Throw(
        git_describe_options_init(&this->raw, GIT_DESCRIBE_OPTIONS_VERSION)
    );
#endif
  }
  ~DescribeOptions() noexcept = default;

  DescribeOptions(const DescribeOptions&) = default;
  DescribeOptions& operator=(const DescribeOptions&) = default;
  DescribeOptions(DescribeOptions&&) = default;
  DescribeOptions& operator=(DescribeOptions&&) = default;

  DescribeOptions& maxCandidatesTags(const unsigned int max) {
    this->raw.max_candidates_tags = max;
    return *this;
  }

  /// Sets the reference lookup strategy
  ///
  /// This behaves like the `--tags` option to git-describe.
  DescribeOptions& describeTags() {
    this->raw.describe_strategy = GIT_DESCRIBE_TAGS;
    return *this;
  }

  /// Sets the reference lookup strategy
  ///
  /// This behaves like the `--all` option to git-describe.
  DescribeOptions& describeAll() {
    this->raw.describe_strategy = GIT_DESCRIBE_ALL;
    return *this;
  }

  /// Indicates when calculating the distance from the matching tag or
  /// reference whether to only walk down the first-parent ancestry.
  DescribeOptions& onlyFollowFirstParent(const bool follow) {
    this->raw.only_follow_first_parent = follow;
    return *this;
  }

  /// If no matching tag or reference is found whether a describe option would
  /// normally fail. This option indicates, however, that it will instead fall
  /// back to showing the full id of the commit.
  DescribeOptions& showCommitOidAsFallback(const bool show) {
    this->raw.show_commit_oid_as_fallback = show;
    return *this;
  }

  DescribeOptions& pattern(const StringRef pattern) {
    this->raw.pattern = pattern.data();
    return *this;
  }
};

struct DescribeFormatOptions {
  git_describe_format_options raw;

  DescribeFormatOptions() {
#if (LIBGIT2_VER_MAJOR < 1) && (LIBGIT2_VER_MINOR < 99)
    git2Throw(git_describe_init_format_options(
        &this->raw, GIT_DESCRIBE_FORMAT_OPTIONS_VERSION
    ));
#else
    git2Throw(git_describe_format_options_init(
        &this->raw, GIT_DESCRIBE_FORMAT_OPTIONS_VERSION
    ));
#endif
  }
  ~DescribeFormatOptions() = default;

  DescribeFormatOptions(const DescribeFormatOptions&) = default;
  DescribeFormatOptions& operator=(const DescribeFormatOptions&) = default;
  DescribeFormatOptions(DescribeFormatOptions&&) = default;
  DescribeFormatOptions& operator=(DescribeFormatOptions&&) = default;

  /// Sets the size of the abbreviated commit id to use.
  ///
  /// The value is the lower bound for the length of the abbreviated string,
  /// and the default is 7.
  DescribeFormatOptions& abbreviatedSize(const unsigned int size) {
    this->raw.abbreviated_size = size;
    return *this;
  }

  /// Sets whether or not the long format is used even when a shorter name
  /// could be used.
  DescribeFormatOptions& alwaysUseLongFormat(const bool long_f) {
    this->raw.always_use_long_format = long_f;
    return *this;
  }

  /// If the workdir is dirty and this is set, this string will be appended to
  /// the description string.
  DescribeFormatOptions& dirtySuffix(const StringRef suffix) {
    this->raw.dirty_suffix = suffix.data();
    return *this;
  }
};

struct Describe {
  git_describe_result* raw = nullptr;

  Describe() = default;
  ~Describe() {
    git_describe_result_free(this->raw);
  }

  Describe(const Describe&) = delete;
  Describe& operator=(const Describe&) = delete;
  Describe(Describe&&) = default;
  Describe& operator=(Describe&&) = default;

  Describe& workdir(const Repository& repo, DescribeOptions& opts) {
    git2Throw(git_describe_workdir(&this->raw, repo.raw, &opts.raw));
    return *this;
  }

  std::string format(const DescribeFormatOptions& opts) const {
    git_buf ret = { nullptr, 0, 0 };
    git2Throw(git_describe_format(&ret, this->raw, &opts.raw));
    return std::string(ret.ptr, ret.size);
  }
};

} // namespace git2
