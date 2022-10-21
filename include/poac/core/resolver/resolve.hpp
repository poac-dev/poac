#ifndef POAC_CORE_RESOLVER_RESOLVE_HPP_
#define POAC_CORE_RESOLVER_RESOLVE_HPP_

// std
#include <algorithm>
#include <iterator>
#include <utility>

// internal
#include "poac/poac.hpp"
#include "poac/util/meta.hpp"

namespace poac::core::resolver::resolve {

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
using DupDeps = typename DuplicateDeps<W>::type;

struct Package {
  /// Package name
  String name;

  /// Version Requirement
  ///
  /// Sometimes, this is like `1.66.0` or like `>=1.64.0 and <2.0.0`.
  String version_rq;

  bool
  is_conan() const noexcept {
    return name.find("conan::") == 0;
  }
};

inline bool
operator==(const Package& lhs, const Package& rhs) {
  return lhs.name == rhs.name && lhs.version_rq == rhs.version_rq;
}

inline usize
hash_value(const Package& p) {
  usize seed = 0;
  boost::hash_combine(seed, p.name);
  boost::hash_combine(seed, p.version_rq);
  return seed;
}

template <>
struct DuplicateDeps<WithoutDeps> {
  using type = Vec<Package>;
};

using Deps = Option<DupDeps<WithoutDeps>>;

template <>
struct DuplicateDeps<WithDeps> {
  using type = Vec<std::pair<Package, Deps>>;
};

template <typename W>
using UniqDeps = std::conditional_t<
    W::value, HashMap<Package, Deps>,
    // <name, ver_req>
    HashMap<String, String>>;

inline const Package&
get_package(const UniqDeps<WithDeps>::value_type& deps) noexcept {
  return deps.first;
}

inline String
to_binary_numbers(const i32& x, const usize& digit) {
  return format("{:0{}b}", x, digit);
}

// A ∨ B ∨ C
// A ∨ ¬B ∨ ¬C
// ¬A ∨ B ∨ ¬C
// ¬A ∨ ¬B ∨ C
// ¬A ∨ ¬B ∨ ¬C
Vec<Vec<i32>>
multiple_versions_cnf(const Vec<i32>& clause);

Vec<Vec<i32>>
create_cnf(const DupDeps<WithDeps>& activated);

[[nodiscard]] Result<UniqDeps<WithDeps>, String>
solve_sat(const DupDeps<WithDeps>& activated, const Vec<Vec<i32>>& clauses);

[[nodiscard]] Result<UniqDeps<WithDeps>, String>
backtrack_loop(const DupDeps<WithDeps>& activated);

template <typename SinglePassRange>
bool
duplicate_loose(const SinglePassRange& rng) {
  const auto first = std::begin(rng);
  const auto last = std::end(rng);
  return std::find_if(
             first, last,
             [&](const auto& x) {
               return std::count_if(
                          first, last,
                          [&](const auto& y) {
                            return get_package(x).name == get_package(y).name;
                          }
                      )
                      > 1;
             }
         )
         != last;
}

// Interval to multiple versions
// `>=0.1.2 and <3.4.0` -> { 2.4.0, 2.5.0 }
// name is boost/config, no boost-config
[[nodiscard]] Result<Vec<String>, String>
get_versions_satisfy_interval(const Package& package);

struct Cache {
  Package package;

  /// versions in the interval
  Vec<String> versions;
};

inline bool
operator==(const Cache& lhs, const Cache& rhs) {
  return lhs.package == rhs.package && lhs.versions == rhs.versions;
}

inline usize
hash_value(const Cache& i) {
  usize seed = 0;
  boost::hash_combine(seed, i.package);
  boost::hash_range(seed, i.versions.begin(), i.versions.end());
  return seed;
}

using IntervalCache = HashSet<Cache>;

inline bool
cache_exists(const IntervalCache& cache, const Package& package) {
  return util::meta::find_if(cache, [&package](const Cache& c) {
    return c.package == package;
  });
}

inline bool
cache_exists(const DupDeps<WithDeps>& deps, const Package& package) {
  return util::meta::find_if(deps, [&package](const auto& c) {
    return get_package(c) == package;
  });
}

DupDeps<WithoutDeps>
gather_deps_of_deps(
    const UniqDeps<WithoutDeps>& deps_api_res, IntervalCache& interval_cache
);

void
gather_deps(
    const Package& package, DupDeps<WithDeps>& new_deps,
    IntervalCache& interval_cache
);

[[nodiscard]] Result<DupDeps<WithDeps>, String>
gather_all_deps(const UniqDeps<WithoutDeps>& deps);

} // namespace poac::core::resolver::resolve

#endif // POAC_CORE_RESOLVER_RESOLVE_HPP_
