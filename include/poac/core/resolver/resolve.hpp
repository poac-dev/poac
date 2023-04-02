#pragma once

// std
#include <algorithm>
#include <iterator>
#include <utility>

// internal
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/meta.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

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
using DupDeps = typename DuplicateDeps<W>::Type;

struct DependencyInfo {
  /// Version Requirement
  ///
  /// Sometimes, this is like `1.66.0` or like `>=1.64.0 and <2.0.0`.
  String version_rq;
};

inline Fn operator==(const DependencyInfo& lhs, const DependencyInfo& rhs)
    ->bool {
  return lhs.version_rq == rhs.version_rq;
}

inline Fn hash_value(const DependencyInfo& d)->usize {
  usize seed = 0;
  boost::hash_combine(seed, d.version_rq);
  return seed;
}

// NOLINTNEXTLINE(bugprone-exception-escape)
struct Package {
  /// Package name
  String name;

  DependencyInfo dep_info;
};

inline Fn operator==(const Package& lhs, const Package& rhs)->bool {
  return lhs.name == rhs.name && lhs.dep_info == rhs.dep_info;
}

inline Fn hash_value(const Package& p)->usize {
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

inline Fn get_package(const UniqDeps<WithDeps>::value_type& deps) noexcept
    -> const Package& {
  return deps.first;
}

inline Fn to_binary_numbers(const i32& x, const usize& digit)->String {
  return format("{:0{}b}", x, digit);
}

// A ∨ B ∨ C
// A ∨ ¬B ∨ ¬C
// ¬A ∨ B ∨ ¬C
// ¬A ∨ ¬B ∨ C
// ¬A ∨ ¬B ∨ ¬C
Fn multiple_versions_cnf(const Vec<i32>& clause)->Vec<Vec<i32>>;

Fn create_cnf(const DupDeps<WithDeps>& activated)->Vec<Vec<i32>>;

[[nodiscard]] Fn
solve_sat(const DupDeps<WithDeps>& activated, const Vec<Vec<i32>>& clauses)
    ->Result<UniqDeps<WithDeps>, String>;

[[nodiscard]] Fn backtrack_loop(const DupDeps<WithDeps>& activated)
    ->Result<UniqDeps<WithDeps>, String>;

template <typename SinglePassRange>
Fn duplicate_loose(const SinglePassRange& rng)->bool {
  Let first = std::begin(rng);
  Let last = std::end(rng);
  return std::find_if(
             first, last,
             [&](Let& x) {
               return std::count_if(
                          first, last,
                          [&](Let& y) {
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
[[nodiscard]] Fn get_versions_satisfy_interval(const Package& package)
    ->Result<Vec<String>, String>;

// NOLINTNEXTLINE(bugprone-exception-escape)
struct Cache {
  Package package;

  /// versions in the interval
  Vec<String> versions;
};

inline Fn operator==(const Cache& lhs, const Cache& rhs)->bool {
  return lhs.package == rhs.package && lhs.versions == rhs.versions;
}

inline Fn hash_value(const Cache& i)->usize {
  usize seed = 0;
  boost::hash_combine(seed, i.package);
  boost::hash_range(seed, i.versions.begin(), i.versions.end());
  return seed;
}

using IntervalCache = HashSet<Cache>;

inline Fn cache_exists(const IntervalCache& cache, const Package& package)
    ->bool {
  return util::meta::find_if(cache, [&package](const Cache& c) {
    return c.package == package;
  });
}

inline Fn cache_exists(const DupDeps<WithDeps>& deps, const Package& package)
    ->bool {
  return util::meta::find_if(deps, [&package](Let& c) {
    return get_package(c) == package;
  });
}

Fn gather_deps_of_deps(
    const UniqDeps<WithoutDeps>& deps_api_res, IntervalCache& interval_cache
)
    ->DupDeps<WithoutDeps>;

void gather_deps(
    const Package& package, DupDeps<WithDeps>& new_deps,
    IntervalCache& interval_cache
);

[[nodiscard]] Fn gather_all_deps(const UniqDeps<WithoutDeps>& deps)
    ->Result<DupDeps<WithDeps>, String>;

} // namespace poac::core::resolver::resolve
