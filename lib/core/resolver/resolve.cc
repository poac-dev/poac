// external
#include <boost/dynamic_bitset.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/irange.hpp>

// internal
#include "poac/core/resolver/resolve.hpp"
#include "poac/core/resolver/sat.hpp"
#include "poac/util/net.hpp"
#include "poac/util/semver/semver.hpp"
#include "poac/util/verbosity.hpp"

namespace poac::core::resolver::resolve {

// A ∨ B ∨ C
// A ∨ ¬B ∨ ¬C
// ¬A ∨ B ∨ ¬C
// ¬A ∨ ¬B ∨ C
// ¬A ∨ ¬B ∨ ¬C
Fn multiple_versions_cnf(const Vec<i32>& clause)->Vec<Vec<i32>> {
  return boost::irange(0, 1 << clause.size()) // number of combinations
         | boost::adaptors::transformed([&clause](const i32 i) {
             return boost::dynamic_bitset<>(to_binary_numbers(i, clause.size())
             );
           })
         | boost::adaptors::filtered([](const boost::dynamic_bitset<>& bs) {
             return bs.count() != 1;
           })
         | boost::adaptors::transformed(
             [&clause](const boost::dynamic_bitset<>& bs) -> Vec<i32> {
               return boost::irange(usize{0}, bs.size())
                      | boost::adaptors::transformed([&clause,
                                                      &bs](const i32 i) {
                          return bs[i] ? clause[i] * -1 : clause[i];
                        })
                      | util::meta::CONTAINERIZED;
             }
         )
         | util::meta::CONTAINERIZED;
}

Fn
// NOLINTNEXTLINE(readability-function-cognitive-complexity)
create_cnf(const DupDeps<WithDeps>& activated)
    ->Vec<Vec<i32>> {
  Vec<Vec<i32>> clauses;
  Vec<i32> already_added;

  auto first = std::cbegin(activated);
  auto last = std::cend(activated);
  for (usize i = 0; i < activated.size(); ++i) {
    if (util::meta::find(already_added, i)) {
      continue;
    }

    Let name_lambda = [&](Let& x) {
      return get_package(x) == get_package(activated[i]);
    };
    // No other packages with the same name as the package currently pointed to
    // exist
    if (const i64 count = std::count_if(first, last, name_lambda); count == 1) {
      Vec<i32> clause;
      clause.emplace_back(i + 1);
      clauses.emplace_back(clause);

      // index ⇒ deps
      if (!activated[i].second.has_value()) {
        clause[0] *= -1;
        for (Let & [ name, dep_info ] : activated[i].second.value()) {
          // It is guaranteed to exist
          clause.emplace_back(
              util::meta::index_of_if(
                  first, last,
                  [&n = name, &di = dep_info](Let& d) {
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
      Vec<i32> clause;

      for (auto found = first; found != last;
           found = std::find_if(found, last, name_lambda)) {
        const i64 index = std::distance(first, found);
        clause.emplace_back(index + 1);
        already_added.emplace_back(index + 1);

        // index ⇒ deps
        if (!found->second.has_value()) {
          Vec<i32> new_clause;
          new_clause.emplace_back(index);
          for (const Package& package : found->second.value()) {
            // It is guaranteed to exist
            new_clause.emplace_back(
                util::meta::index_of_if(
                    first, last,
                    [&package](Let& p) {
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

[[nodiscard]] Fn
solve_sat(const DupDeps<WithDeps>& activated, const Vec<Vec<i32>>& clauses)
    ->Result<UniqDeps<WithDeps>, String> {
  // deps.activated.size() == variables
  const Vec<i32> assignments = Try(sat::solve(clauses, activated.size()));
  UniqDeps<WithDeps> resolved_deps{};
  log::debug("SAT");
  for (i32 a : assignments) {
    log::debug("{} ", a);
    if (a > 0) {
      Let & [ package, deps ] = activated[a - 1];
      resolved_deps.emplace(package, deps);
    }
  }
  log::debug(0);
  return Ok(resolved_deps);
}

[[nodiscard]] Fn backtrack_loop(const DupDeps<WithDeps>& activated)
    ->Result<UniqDeps<WithDeps>, String> {
  const Vec<Vec<i32>> clauses = create_cnf(activated);
  if (util::verbosity::is_verbose()) {
    for (const Vec<i32>& c : clauses) {
      for (i32 l : c) {
        Let& deps = activated[std::abs(l) - 1];
        const Package package = get_package(deps);
        log::debug("{}-{}: {}, ", package.name, package.dep_info.version_rq, l);
      }
      log::debug("");
    }
  }
  return solve_sat(activated, clauses);
}

// Interval to multiple versions
// `>=0.1.2 and <3.4.0` -> { 2.4.0, 2.5.0 }
// name is boost/config, no boost-config
[[nodiscard]] Fn get_versions_satisfy_interval(const Package& package)
    ->Result<Vec<String>, String> {
  // TODO(ken-matsui): (`>1.2 and <=1.3.2` -> NG，`>1.2.0-alpha and <=1.3.2` ->
  // OK) `2.0.0` specific version or `>=0.1.2 and <3.4.0` version interval
  const semver::Interval i(package.dep_info.version_rq);
  const Vec<String> satisfied_versions =
      Try(util::net::api::versions(package.name))
      | boost::adaptors::filtered([&i](StringRef s) { return i.satisfies(s); })
      | util::meta::CONTAINERIZED;

  if (satisfied_versions.empty()) {
    return Err(format(
        "`{}: {}` not found; seem dependencies are broken", package.name,
        package.dep_info.version_rq
    ));
  }
  return Ok(satisfied_versions);
}

Fn gather_deps_of_deps(
    const UniqDeps<WithoutDeps>& deps_api_res, IntervalCache& interval_cache
)
    ->DupDeps<WithoutDeps> {
  DupDeps<WithoutDeps> cur_deps_deps;
  for (Let & [ name, dep_info ] : deps_api_res) {
    const Package package{name, dep_info};

    // Check if node package is resolved dependency (by interval)
    Let found_cache =
        boost::range::find_if(interval_cache, [&package](const Cache& cache) {
          return package == cache.package;
        });

    const Vec<String> dep_versions =
        found_cache != interval_cache.cend()
            ? found_cache->versions
            : get_versions_satisfy_interval(package).unwrap();
    if (found_cache == interval_cache.cend()) {
      // Cache interval and versions pair
      interval_cache.emplace(Cache{package, dep_versions});
    }
    for (const String& dep_version : dep_versions) {
      cur_deps_deps.emplace_back(Package{package.name, dep_version});
    }
  }
  return cur_deps_deps;
}

void gather_deps(
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
    new_deps.emplace_back(package, None);
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

[[nodiscard]] Fn gather_all_deps(const UniqDeps<WithoutDeps>& deps)
    ->Result<DupDeps<WithDeps>, String> {
  DupDeps<WithDeps> duplicate_deps;
  IntervalCache interval_cache;

  // Activate the root of dependencies
  for (Let & [ name, dep_info ] : deps) {
    const Package package{name, dep_info};

    // We don't resolve deps of conan packages, this is defer to conan itself
    if (package.is_conan()) {
      duplicate_deps.emplace_back(package, None);
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
    const Vec<String> versions = Try(get_versions_satisfy_interval(package));
    // Cache interval and versions pair
    interval_cache.emplace(Cache{package, versions});
    for (const String& version : versions) {
      gather_deps(
          Package{package.name, version}, duplicate_deps, interval_cache
      );
    }
  }
  return Ok(duplicate_deps);
}

} // namespace poac::core::resolver::resolve
