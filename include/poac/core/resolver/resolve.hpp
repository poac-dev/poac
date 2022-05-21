#ifndef POAC_CORE_RESOLVER_RESOLVE_HPP
#define POAC_CORE_RESOLVER_RESOLVE_HPP

// std
#include <cmath>
#include <iostream>
#include <stack>
#include <sstream>
#include <regex>
#include <utility>
#include <algorithm>
#include <iterator>

// external
#include <boost/dynamic_bitset.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/algorithm_ext/push_back.hpp>
#include <boost/range/adaptor/filtered.hpp>
#include <boost/range/adaptor/transformed.hpp>
#include <boost/range/irange.hpp>
#include <boost/range/join.hpp>
#include <spdlog/spdlog.h>

// internal
#include <poac/poac.hpp>
#include <poac/config.hpp>
#include <poac/core/resolver/sat.hpp>
#include <poac/util/meta.hpp>
#include <poac/util/net.hpp>
#include <poac/util/semver/semver.hpp>
#include <poac/util/verbosity.hpp>

namespace poac::core::resolver::resolve {
    struct with_deps : std::true_type {};
    struct without_deps : std::false_type {};

    // Duplicate dependencies mean not resolved,
    //   so a package has version rather than interval generally.
    // A package depends on packages have many different versions of one name,
    //   so this should not be std::unordered_map.
    // Package information does not need dependencies' dependencies,
    //   so second value of std::pair is not Package (this type)
    //   just needs std::string indicated specific version.
    template <class WithDeps>
    struct duplicate_deps {};

    template <class WithDeps>
    using duplicate_deps_t = typename duplicate_deps<WithDeps>::type;

    template <>
    struct duplicate_deps<without_deps> {
        using type = Vec<std::pair<String, String>>;
    };

    using package_t = std::pair<
        String, // name
        String // version or interval
    >;

    inline package_t::first_type&
    get_name(package_t& package) noexcept {
        return package.first;
    }

    inline const package_t::first_type&
    get_name(const package_t& package) noexcept {
        return package.first;
    }

    inline package_t::second_type&
    get_version(package_t& package) noexcept {
        return package.second;
    }

    inline const package_t::second_type&
    get_version(const package_t& package) noexcept {
        return package.second;
    }

    inline package_t::second_type&
    get_interval(package_t& package) noexcept {
        return get_version(package);
    }

    inline const package_t::second_type&
    get_interval(const package_t& package) noexcept {
        return get_version(package);
    }

    using deps_t = Option<duplicate_deps_t<without_deps>>;

    template <>
    struct duplicate_deps<with_deps> {
        using type = Vec<std::pair<package_t, deps_t>>;
    };

    struct hash_pair {
        template <class T, class U>
        usize operator()(const std::pair<T, U>& p) const {
            return std::hash<T>()(p.first) ^ std::hash<U>()(p.second);
        }
    };

    template <class WithDeps>
    using unique_deps_t =
        std::conditional_t<
            WithDeps::value,
            HashMap<package_t, deps_t, hash_pair>,
            // <name, version or interval>
            HashMap<String, String>
        >;

    inline const package_t&
    get_package(const unique_deps_t<with_deps>::value_type& deps) noexcept {
        return deps.first;
    }

    String to_binary_numbers(const int& x, const usize& digit) {
        return format("{:0{}b}", x, digit);
    }

    // A ∨ B ∨ C
    // A ∨ ¬B ∨ ¬C
    // ¬A ∨ B ∨ ¬C
    // ¬A ∨ ¬B ∨ C
    // ¬A ∨ ¬B ∨ ¬C
    Vec<Vec<int>>
    multiple_versions_cnf(const Vec<int>& clause) {
        return boost::irange(0, 1 << clause.size()) // number of combinations
            | boost::adaptors::transformed(
                  [&clause](const auto& i){
                      return boost::dynamic_bitset<>(
                          to_binary_numbers(i, clause.size())
                      );
                  }
              )
            | boost::adaptors::filtered(
                  [](const boost::dynamic_bitset<>& bs){
                      return bs.count() != 1;
                  }
              )
            | boost::adaptors::transformed(
                  [&clause](const boost::dynamic_bitset<>& bs) -> Vec<int> {
                      return
                          boost::irange(usize{0}, bs.size())
                          | boost::adaptors::transformed(
                                [&clause, &bs](const auto& i){
                                    return bs[i] ? clause[i] * -1 : clause[i];
                                }
                            )
                          | util::meta::containerized;
                  }
              )
            | util::meta::containerized;
    }

    Vec<Vec<i32>>
    create_cnf(const duplicate_deps_t<with_deps>& activated) {
        Vec<Vec<i32>> clauses;
        Vec<i32> already_added;

        auto first = std::cbegin(activated);
        auto last = std::cend(activated);
        for (i32 i = 0; i < static_cast<i32>(activated.size()); ++i) {
            if (util::meta::find(already_added, i)) {
                continue;
            }

            const auto name_lambda = [&](const auto& x){
                return x.first == activated[i].first;
            };
            // No other packages with the same name as the package currently pointed to exist
            if (const auto count = std::count_if(first, last, name_lambda); count == 1) {
                Vec<i32> clause;
                clause.emplace_back(i + 1);
                clauses.emplace_back(clause);

                // index ⇒ deps
                if (!activated[i].second.has_value()) {
                    clause[0] *= -1;
                    for (const auto& [name, version] : activated[i].second.value()) {
                        // It is guaranteed to exist
                        clause.emplace_back(
                            util::meta::index_of_if(
                                first, last,
                                [&n=name, &v=version](const auto& d){
                                    return d.first.first == n &&
                                           d.first.second == v;
                                }) + 1);
                    }
                    clauses.emplace_back(clause);
                }
            } else if (count > 1) {
                Vec<i32> clause;

                for (auto found = first; found != last; found = std::find_if(found, last, name_lambda)) {
                    const auto index = std::distance(first, found);
                    clause.emplace_back(index + 1);
                    already_added.emplace_back(index + 1);

                    // index ⇒ deps
                    if (!found->second.has_value()) {
                        Vec<i32> new_clause;
                        new_clause.emplace_back(index);
                        for (const auto& package : found->second.value()) {
                            // It is guaranteed to exist
                            new_clause.emplace_back(
                                util::meta::index_of_if(
                                    first, last,
                                    [&package=package](const auto& p){
                                        return p.first.first == package.first &&
                                               p.first.second == package.second;
                                    }
                                ) + 1
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

    [[nodiscard]] Result<unique_deps_t<with_deps>, String>
    solve_sat(const duplicate_deps_t<with_deps>& activated, const Vec<Vec<i32>>& clauses) {
        // deps.activated.size() == variables
        const Vec<i32> assignments = tryi(sat::solve(clauses, activated.size()));
        unique_deps_t<with_deps> resolved_deps{};
        spdlog::debug("SAT");
        for (const auto& a : assignments) {
            spdlog::debug("{} ", a);
            if (a > 0) {
                const auto& [package, deps] = activated[a - 1];
                resolved_deps.emplace(package, deps);
            }
        }
        spdlog::debug(0);
        return Ok(resolved_deps);
    }

    [[nodiscard]] Result<unique_deps_t<with_deps>, String>
    backtrack_loop(const duplicate_deps_t<with_deps>& activated) {
        const auto clauses = create_cnf(activated);
        if (util::verbosity::is_verbose()) {
            for (const auto& c : clauses) {
                for (const auto& l : c) {
                    const auto deps = activated[std::abs(l) - 1];
                    spdlog::debug(
                        "{}-{}: {}, ",
                        get_name(get_package(deps)),
                        get_version(get_package(deps)),
                        l
                    );
                }
                spdlog::debug("");
            }
        }
        return solve_sat(activated, clauses);
    }

    template <class SinglePassRange>
    bool duplicate_loose(const SinglePassRange& rng) {
        const auto first = std::begin(rng);
        const auto last = std::end(rng);
        return std::find_if(first, last, [&](const auto& x){
            return std::count_if(first, last, [&](const auto& y) {
                return get_name(get_package(x)) == get_name(get_package(y));
            }) > 1;
        }) != last;
    }

    // Interval to multiple versions
    // `>=0.1.2 and <3.4.0` -> { 2.4.0, 2.5.0 }
    // `latest` -> { 2.5.0 }: (removed)
    // name is boost/config, no boost-config
    [[nodiscard]] Result<Vec<String>, String>
    get_versions_satisfy_interval(const package_t& package) {
        // TODO: (`>1.2 and <=1.3.2` -> NG，`>1.2.0-alpha and <=1.3.2` -> OK)
        // `2.0.0` specific version or `>=0.1.2 and <3.4.0` version interval
        const semver::Interval i(get_interval(package));
        const Vec<String> satisfied_versions =
            tryi(util::net::api::versions(get_name(package)))
            | boost::adaptors::filtered(
                [&i](StringRef s){ return i.satisfies(s); }
            )
            | util::meta::containerized;

        if (satisfied_versions.empty()) {
            return Err(format(
                "`{}: {}` not found; seem dependencies are broken",
                get_name(package), get_interval(package)
            ));
        }
        return Ok(satisfied_versions);
    }

    using interval_cache_t =
        Vec<
            std::tuple<
                package_t,
                Vec<String> // versions in the interval
            >>;

    inline const package_t&
    get_package(const interval_cache_t::value_type& cache) noexcept {
        return std::get<0>(cache);
    }

    inline const Vec<String>&
    get_versions(const interval_cache_t::value_type& cache) noexcept {
        return std::get<1>(cache);
    }

    inline bool
    exist_cache_impl(const package_t& a, const package_t& b) noexcept {
        return get_name(a) == get_name(b)
            && get_version(a) == get_version(b);
    }

    template <class Range>
    inline bool
    exist_cache(Range&& cache, const package_t& package) {
        return util::meta::find_if(
            std::forward<Range>(cache),
            [&package](const auto& c) {
                return exist_cache_impl(package, get_package(c));
            }
        );
    }

    duplicate_deps_t<without_deps>
    gather_deps_of_deps(
        const unique_deps_t<without_deps>& deps_api_res,
        interval_cache_t& interval_cache)
    {
        duplicate_deps_t<without_deps> cur_deps_deps;
        for (const auto& package : deps_api_res) {
            // Check if node package is resolved dependency (by interval)
            const auto found_cache =
                boost::range::find_if(
                    interval_cache,
                    [&package](const auto& cache) {
                        return exist_cache_impl(package, get_package(cache));
                    }
                );

            const auto dep_versions =
                found_cache != interval_cache.cend()
                    ? get_versions(*found_cache)
                    : get_versions_satisfy_interval(package).unwrap();
            if (found_cache == interval_cache.cend()) {
                // Cache interval and versions pair
                interval_cache.emplace_back(package, dep_versions);
            }
            for (const auto& dep_version : dep_versions) {
                cur_deps_deps.emplace_back(get_name(package), dep_version);
            }
        }
        return cur_deps_deps;
    }

    void gather_deps(
        const package_t& package,
        duplicate_deps_t<with_deps>& new_deps,
        interval_cache_t& interval_cache)
    {
        // Check if root package resolved dependency
        //   (whether the specific version is the same),
        //   and check circulating
        if (exist_cache(new_deps, package)) {
            return;
        }

        // Get dependencies of dependencies
        const unique_deps_t<without_deps> deps_api_res =
            util::net::api::deps(get_name(package), get_version(package)).unwrap();
        if (deps_api_res.empty()) {
            new_deps.emplace_back(package, None);
        } else {
            const auto deps_of_deps = gather_deps_of_deps(deps_api_res, interval_cache);

            // Store dependency and the dependency's dependencies.
            new_deps.emplace_back(package, deps_of_deps);

            // Gather dependencies of dependencies of dependencies.
            for (const auto& dep_package : deps_of_deps) {
                gather_deps(dep_package, new_deps, interval_cache);
            }
        }
    }

    [[nodiscard]] Result<duplicate_deps_t<with_deps>, String>
    gather_all_deps(const unique_deps_t<without_deps>& deps) {
        duplicate_deps_t<with_deps> duplicate_deps;
        interval_cache_t interval_cache;

        // Activate the root of dependencies
        for (const auto& package : deps) {
            // Check whether the packages specified in poac.toml
            //   are already resolved which includes
            //   that package's dependencies and package's versions
            //   by checking whether package's interval is the same.
            if (exist_cache(interval_cache, package)) {
                continue;
            }

            // Get versions using interval
            // FIXME: versions API and deps API are received the almost same responses
            const Vec<String> versions =
                tryi(get_versions_satisfy_interval(package));
            // Cache interval and versions pair
            interval_cache.emplace_back(package, versions);
            for (const auto& version : versions) {
                gather_deps(
                    std::make_pair(get_name(package), version),
                    duplicate_deps,
                    interval_cache
                );
            }
        }
        return Ok(duplicate_deps);
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_RESOLVE_HPP
