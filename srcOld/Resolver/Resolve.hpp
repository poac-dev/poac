module;

// std
#include <algorithm>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

// external
#include <boost/dynamic_bitset.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/irange.hpp>

// internal
#include "../../util/result-macros.hpp"

export module poac.core.resolver.resolve;

import poac.util.format;
import poac.util.log;
import poac.util.meta;
import poac.util.result;
import poac.util.rustify;
import poac.core.resolver.sat;
import poac.core.resolver.types;
import poac.util.net;
import poac.util.registry.conan.v1.resolver;
import semver;
import poac.util.verbosity;

export namespace poac::core::resolver::resolve {

inline auto
get_package(const UniqDeps<WithDeps>::value_type& deps
) noexcept -> const Package& {
  return deps.first;
}

inline auto
to_binary_numbers(const i32& x, const usize& digit) -> std::string {
  return format("{:0{}b}", x, digit);
}

// A ∨ B ∨ C
// A ∨ ¬B ∨ ¬C
// ¬A ∨ B ∨ ¬C
// ¬A ∨ ¬B ∨ C
// ¬A ∨ ¬B ∨ ¬C
auto
multiple_versions_cnf(const std::vector<i32>& clause
) -> std::vector<std::vector<i32>> {
  return boost::irange(0, 1 << clause.size()) // number of combinations
         | boost::adaptors::transformed([&clause](const i32 i) {
             return boost::dynamic_bitset<>(to_binary_numbers(i, clause.size())
             );
           })
         | boost::adaptors::filtered([](const boost::dynamic_bitset<>& bs) {
             return bs.count() != 1;
           })
         | boost::adaptors::transformed(
             [&clause](const boost::dynamic_bitset<>& bs) -> std::vector<i32> {
               return boost::irange(usize{ 0 }, bs.size())
                      | boost::adaptors::transformed([&clause,
                                                      &bs](const i32 i) {
                          return bs[i] ? clause[i] * -1 : clause[i];
                        })
                      | util::meta::CONTAINERIZED;
             }
         )
         | util::meta::CONTAINERIZED;
}

auto
create_cnf(const DupDeps<WithDeps>& activated
) -> std::vector<std::vector<i32>> {
  std::vector<std::vector<i32>> clauses;
  std::vector<i32> already_added;

  auto first = std::cbegin(activated);
  auto last = std::cend(activated);
  for (usize i = 0; i < activated.size(); ++i) {
    if (util::meta::find(already_added, i)) {
      continue;
    }

    const auto name_lambda = [&](const auto& x) {
      return get_package(x) == get_package(activated[i]);
    };
    // No other packages with the same name as the package currently pointed to
    // exist
    if (const i64 count = std::count_if(first, last, name_lambda); count == 1) {
      std::vector<i32> clause;
      clause.emplace_back(i + 1);
      clauses.emplace_back(clause);

      // index ⇒ deps
      if (!activated[i].second.has_value()) {
        clause[0] *= -1;
        for (const auto& [name, dep_info] : activated[i].second.value()) {
          // It is guaranteed to exist
          clause.emplace_back(
              util::meta::index_of_if(
                  first, last,
                  [&n = name, &di = dep_info](const auto& d) {
                    return get_package(d).name == n
                           && get_package(d).dep_info == di;
                  }
              )
              + 1
          );
        }
        clauses.emplace_back(clause);
      }
    } else if (count > 1) {
      std::vector<i32> clause;

      for (auto found = first; found != last;
           found = std::find_if(found, last, name_lambda)) {
        const i64 index = std::distance(first, found);
        clause.emplace_back(index + 1);
        already_added.emplace_back(index + 1);

        // index ⇒ deps
        if (!found->second.has_value()) {
          std::vector<i32> new_clause;
          new_clause.emplace_back(index);
          for (const Package& package : found->second.value()) {
            // It is guaranteed to exist
            new_clause.emplace_back(
                util::meta::index_of_if(
                    first, last,
                    [&package](const auto& p) {
                      return get_package(p).name == package.name
                             && get_package(p).dep_info == package.dep_info;
                    }
                )
                + 1
            );
          }
          clauses.emplace_back(new_clause);
        }
        ++found;
      }
      boost::range::push_back(clauses, multiple_versions_cnf(clause));
    }
  }
  return clauses;
}

[[nodiscard]] auto
solve_sat(
    const DupDeps<WithDeps>& activated,
    const std::vector<std::vector<i32>>& clauses
) -> Result<UniqDeps<WithDeps>, std::string> {
  // deps.activated.size() == variables
  const std::vector<i32> assignments =
      Try(sat::solve(clauses, activated.size()));
  UniqDeps<WithDeps> resolved_deps{};
  log::debug("SAT");
  for (i32 a : assignments) {
    log::debug("{} ", a);
    if (a > 0) {
      const auto& [package, deps] = activated[a - 1];
      resolved_deps.emplace(package, deps);
    }
  }
  log::debug(0);
  return Ok(resolved_deps);
}

[[nodiscard]] auto
backtrack_loop(const DupDeps<WithDeps>& activated
) -> Result<UniqDeps<WithDeps>, std::string> {
  const std::vector<std::vector<i32>> clauses = create_cnf(activated);
  if (util::verbosity::is_verbose()) {
    for (const std::vector<i32>& c : clauses) {
      for (i32 l : c) {
        const auto& deps = activated[std::abs(l) - 1];
        const Package package = get_package(deps);
        log::debug("{}-{}: {}, ", package.name, package.dep_info.version_rq, l);
      }
      log::debug("");
    }
  }
  return solve_sat(activated, clauses);
}

template <typename SinglePassRange>
auto
duplicate_loose(const SinglePassRange& rng) -> bool {
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
[[nodiscard]] auto
get_versions_satisfy_interval(const Package& package
) -> Result<std::vector<std::string>, std::string> {
  // TODO(ken-matsui): (`>1.2 and <=1.3.2` -> NG，`>1.2.0-alpha and <=1.3.2` ->
  // OK) `2.0.0` specific version or `>=0.1.2 and <3.4.0` version interval
  const semver::Interval i(package.dep_info.version_rq);
  const std::vector<std::string> satisfied_versions =
      Try(util::net::api::versions(package.name))
      | boost::adaptors::filtered([&i](std::string_view s) {
          return i.satisfies(s);
        })
      | util::meta::CONTAINERIZED;

  if (satisfied_versions.empty()) {
    return Err(format(
        "`{}: {}` not found; seem dependencies are broken", package.name,
        package.dep_info.version_rq
    ));
  }
  return Ok(satisfied_versions);
}

// NOLINTNEXTLINE(bugprone-exception-escape)
struct Cache {
  Package package;

  /// versions in the interval
  std::vector<std::string> versions;
};

inline auto
operator==(const Cache& lhs, const Cache& rhs) -> bool {
  return lhs.package == rhs.package && lhs.versions == rhs.versions;
}

inline auto
hash_value(const Cache& i) -> usize {
  usize seed = 0;
  boost::hash_combine(seed, i.package);
  boost::hash_range(seed, i.versions.begin(), i.versions.end());
  return seed;
}

using IntervalCache = std::unordered_set<Cache>;

inline auto
cache_exists(const IntervalCache& cache, const Package& package) -> bool {
  return util::meta::find_if(cache, [&package](const Cache& c) {
    return c.package == package;
  });
}

inline auto
cache_exists(const DupDeps<WithDeps>& deps, const Package& package) -> bool {
  return util::meta::find_if(deps, [&package](const auto& c) {
    return get_package(c) == package;
  });
}

auto
gather_deps_of_deps(
    const UniqDeps<WithoutDeps>& deps_api_res, IntervalCache& interval_cache
) -> DupDeps<WithoutDeps> {
  DupDeps<WithoutDeps> cur_deps_deps;
  for (const auto& [name, dep_info] : deps_api_res) {
    const Package package{ name, dep_info };

    // Check if node package is resolved dependency (by interval)
    const auto found_cache =
        boost::range::find_if(interval_cache, [&package](const Cache& cache) {
          return package == cache.package;
        });

    const std::vector<std::string> dep_versions =
        found_cache != interval_cache.cend()
            ? found_cache->versions
            : get_versions_satisfy_interval(package).unwrap();
    if (found_cache == interval_cache.cend()) {
      // Cache interval and versions pair
      interval_cache.emplace(Cache{ package, dep_versions });
    }
    for (const std::string& dep_version : dep_versions) {
      cur_deps_deps.emplace_back(Package{ package.name, dep_version });
    }
  }
  return cur_deps_deps;
}

void
gather_deps(
    const Package& package, DupDeps<WithDeps>& new_deps,
    IntervalCache& interval_cache
) {
  // Check if root package resolved dependency (whether the specific version is
  // the same), and check circulating
  if (cache_exists(new_deps, package)) {
    return;
  }

  // Get dependencies of dependencies
  const UniqDeps<WithoutDeps> deps_api_res =
      util::net::api::deps(package.name, package.dep_info.version_rq).unwrap();
  if (deps_api_res.empty()) {
    new_deps.emplace_back(package, std::nullopt);
  } else {
    const DupDeps<WithoutDeps> deps_of_deps =
        gather_deps_of_deps(deps_api_res, interval_cache);

    // Store dependency and the dependency's dependencies.
    new_deps.emplace_back(package, deps_of_deps);

    // Gather dependencies of dependencies of dependencies.
    for (const Package& dep_package : deps_of_deps) {
      gather_deps(dep_package, new_deps, interval_cache);
    }
  }
}

[[nodiscard]] auto
gather_all_deps(const UniqDeps<WithoutDeps>& deps
) -> Result<DupDeps<WithDeps>, std::string> {
  DupDeps<WithDeps> duplicate_deps;
  IntervalCache interval_cache;

  // Activate the root of dependencies
  for (const auto& [name, dep_info] : deps) {
    const Package package{ name, dep_info };

    // We don't resolve deps of conan packages, this is defer to conan itself
    if (poac::util::registry::conan::v1::resolver::is_conan(package)) {
      duplicate_deps.emplace_back(package, std::nullopt);
      continue;
    }

    // Check whether the packages specified in poac.toml
    //   are already resolved which includes
    //   that package's dependencies and package's versions
    //   by checking whether package's interval is the same.
    if (cache_exists(interval_cache, package)) {
      continue;
    }

    // Get versions using interval
    // FIXME: versions API and deps API are received the almost same responses
    const std::vector<std::string> versions =
        Try(get_versions_satisfy_interval(package));
    // Cache interval and versions pair
    interval_cache.emplace(Cache{ package, versions });
    for (const std::string& version : versions) {
      gather_deps(
          Package{ package.name,
                   { version, package.dep_info.index, package.dep_info.type } },
          duplicate_deps, interval_cache
      );
    }
  }
  return Ok(duplicate_deps);
}

} // namespace poac::core::resolver::resolve
