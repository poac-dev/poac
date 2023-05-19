module;

// std
#include <type_traits>

// external
#include <boost/functional/hash_fwd.hpp>

export module poac.core.resolver.types;

import poac.util.result;
import poac.util.rustify;

export namespace poac::core::resolver::resolve {

struct WithDeps : std::true_type {};
struct WithoutDeps : std::false_type {};

// Duplicate dependencies should have non-resolved dependencies which contains
// package info having `version` rather than interval generally. We should avoid
// using `std::unordered_map` here so that packages with the same name possibly
// store in DuplicateDeps. Package information does not need dependencies'
// dependencies (meaning that flattened), so the second value of std::pair is
// this type rather than Package and just needs std::string indicating a
// specific version.
template <typename W>
struct DuplicateDeps {};

template <typename W>
using DupDeps = typename DuplicateDeps<W>::Type;

// NOLINTNEXTLINE(bugprone-exception-escape)
struct DependencyInfo {
  /// Version Requirement
  ///
  /// Sometimes, this is like `1.66.0` or like `>=1.64.0 and <2.0.0`.
  String version_rq;

  /// Registry Index
  String index;

  /// Package type
  String type;
};

inline auto operator==(const DependencyInfo& lhs, const DependencyInfo& rhs)
    -> bool {
  return lhs.version_rq == rhs.version_rq && lhs.index == rhs.index
         && lhs.type == rhs.type;
}

inline auto hash_value(const DependencyInfo& d) -> usize {
  usize seed = 0;
  boost::hash_combine(seed, d.version_rq);
  boost::hash_combine(seed, d.index);
  boost::hash_combine(seed, d.type);
  return seed;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
struct Package {
  /// Package name
  String name;

  DependencyInfo dep_info;
};

inline auto operator==(const Package& lhs, const Package& rhs) -> bool {
  return lhs.name == rhs.name && lhs.dep_info == rhs.dep_info;
}

inline auto hash_value(const Package& p) -> usize {
  usize seed = 0;
  boost::hash_combine(seed, p.name);
  boost::hash_combine(seed, p.dep_info);
  return seed;
}

template <>
struct DuplicateDeps<WithoutDeps> {
  using Type = Vec<Package>;
};

using Deps = Option<DupDeps<WithoutDeps>>;

template <>
struct DuplicateDeps<WithDeps> {
  using Type = Vec<std::pair<Package, Deps>>;
};

template <typename W>
using UniqDeps = std::conditional_t<
    W::value, HashMap<Package, Deps>,
    // <name, ver_req>
    HashMap<String, DependencyInfo>>;

} // namespace poac::core::resolver::resolve

export namespace poac::core::resolver {

using ResolvedDeps = resolve::UniqDeps<resolve::WithDeps>;

using FailedToParseConfig =
    Error<"parsing the value of the `dependencies` key in poac.toml failed">;
using FailedToResolveDeps = Error<"failed to resolve dependencies">;
using FailedToResolveDepsWithCause =
    Error<"failed to resolve dependencies:\n{}", String>;
using FailedToCreateDirs = Error<"failed to create directories:\n{}", String>;
using FailedToRename =
    Error<"failed to rename a downloaded package: `{}: {}`", String, String>;
using FailedToFetch =
    Error<"failed to fetch a package: `{}: {}`", String, String>;
using IncorrectSha256sum = Error<
    "the sha256sum when published did not match one when downloaded.\n"
    "  published: `{}` != downloaded: `{}\n"
    "Since the downloaded package might contain malicious codes, it "
    "was removed from this PC. We highly recommend submitting an "
    "issue on GitHub of the package and stopping using this package:\n"
    "  {}: {}",
    String, String, String, String>;
using RedefinePredefinedRegistryEntry = Error<
    "Registry entry named `{}` is predefined and can't be overwritten.\n",
    String>;
using DuplicateRegistryEntry =
    Error<"Registry entry named `{}` is duplicated.\n", String>;
using UnknownRegistryType = Error<
    "Registry entry named `{}` has unknown registry type `{}`.\n", String,
    String>;
using Unknown = Error<"unknown error occurred: {}", String>;

} // namespace poac::core::resolver
