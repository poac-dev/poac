#ifndef POAC_CORE_RESOLVER_RESOLVE_HPP
#define POAC_CORE_RESOLVER_RESOLVE_HPP

// std
#include <cmath>
#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <utility>
#include <map>
#include <unordered_map>
#include <optional>
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
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>

// internal
#include <poac/config.hpp>
#include <poac/core/resolver/sat.hpp>
#include <poac/util/meta.hpp>
#include <poac/util/net.hpp>
#include <poac/util/semver/semver.hpp>

namespace poac::core::resolver::resolve {
    struct with_deps : std::true_type {};
    struct without_deps : std::false_type {};

    // Duplicate dependencies mean not resolved,
    //   so a package has version not interval generally.
    // A package depends packages have many different versions of one name,
    //   so this should not be std::unordered_map.
    // Also, package information does not need
    //   package's dependency's dependencies,
    //   so second value of std::pair is not Package (this type)
    //   just needs std::string indicated specific version.
    template <class WithDeps>
    struct duplicate_deps {};

    template <class WithDeps>
    using duplicate_deps_t = typename duplicate_deps<WithDeps>::type;

    template <>
    struct duplicate_deps<without_deps> {
        using type = std::vector<std::pair<std::string, std::string>>;
    };

    using package_t = std::pair<
        std::string, // name
        std::string // version or interval
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

    using deps_t = std::optional<duplicate_deps_t<without_deps>>;

    template <>
    struct duplicate_deps<with_deps> {
        using type = std::vector<std::pair<package_t, deps_t>>;
    };

    struct hash_pair {
        template <class T, class U>
        std::size_t operator()(const std::pair<T, U>& p) const {
            return std::hash<T>()(p.first) ^ std::hash<U>()(p.second);
        }
    };

    template <class WithDeps>
    using unique_deps_t =
        std::conditional_t<
            WithDeps::value,
            std::unordered_map<package_t, deps_t, hash_pair>,
            // <name, version or interval>
            std::unordered_map<std::string, std::string>
        >;

    inline const package_t&
    get_package(const unique_deps_t<with_deps>::value_type& deps) noexcept {
        return deps.first;
    }

    std::string to_binary_numbers(const int& x, const std::size_t& digit) {
        return fmt::format(FMT_STRING("{:0{}b}"), x, digit);
    }

    // A ∨ B ∨ C
    // A ∨ ¬B ∨ ¬C
    // ¬A ∨ B ∨ ¬C
    // ¬A ∨ ¬B ∨ C
    // ¬A ∨ ¬B ∨ ¬C
    std::vector<std::vector<int>>
    multiple_versions_cnf(const std::vector<int>& clause) {
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
                  [&clause](const boost::dynamic_bitset<>& bs) -> std::vector<int> {
                      return
                          boost::irange(std::size_t{0}, bs.size())
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

    std::vector<std::vector<int>>
    create_cnf(const duplicate_deps_t<with_deps>& activated) {
        std::vector<std::vector<int>> clauses;
        std::vector<int> already_added;

        auto first = std::cbegin(activated);
        auto last = std::cend(activated);
        for (int i = 0; i < static_cast<int>(activated.size()); ++i) {
            if (util::meta::find(already_added, i)) {
                continue;
            }

            const auto name_lambda = [&](const auto& x){
                return x.first == activated[i].first;
            };
            // 現在指すパッケージと同名の他のパッケージは存在しない
            if (const auto count = std::count_if(first, last, name_lambda); count == 1) {
                std::vector<int> clause;
                clause.emplace_back(i + 1);
                clauses.emplace_back(clause);

                // index ⇒ deps
                if (!activated[i].second.has_value()) {
                    clause[0] *= -1;
                    for (const auto& [name, version] : activated[i].second.value()) {
                        // 必ず存在することが保証されている
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
                std::vector<int> clause;

                for (auto found = first; found != last; found = std::find_if(found, last, name_lambda)) {
                    const auto index = std::distance(first, found);
                    clause.emplace_back(index + 1);
                    already_added.emplace_back(index + 1);

                    // index ⇒ deps
                    if (!found->second.has_value()) {
                        std::vector<int> new_clause;
                        new_clause.emplace_back(index);
                        for (const auto& package : found->second.value()) {
                            // 必ず存在することが保証されている
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

    [[nodiscard]] mitama::result<unique_deps_t<with_deps>, std::string>
    solve_sat(const duplicate_deps_t<with_deps>& activated, const std::vector<std::vector<int>>& clauses) {
        // deps.activated.size() == variables
        const std::vector<int> assignments = MITAMA_TRY(sat::solve(clauses, activated.size()));
        unique_deps_t<with_deps> resolved_deps{};
        PLOG_DEBUG << "SAT";
        for (const auto& a : assignments) {
            PLOG_DEBUG << a << " ";
            if (a > 0) {
                const auto& [package, deps] = activated[a - 1];
                resolved_deps.emplace(package, deps);
            }
        }
        PLOG_DEBUG << 0;
        return mitama::success(resolved_deps);
    }

    [[nodiscard]] mitama::result<unique_deps_t<with_deps>, std::string>
    backtrack_loop(const duplicate_deps_t<with_deps>& activated) {
        const auto clauses = create_cnf(activated);
        IF_PLOG(plog::debug) {
            for (const auto& c : clauses) {
                for (const auto& l : c) {
                    const auto deps = activated[std::abs(l) - 1];
                    PLOG_DEBUG <<
                        fmt::format(
                            "{}-{}: {}, ",
                            get_name(get_package(deps)),
                            get_version(get_package(deps)),
                            l
                        );
                }
                PLOG_DEBUG << "";
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
    [[nodiscard]] mitama::result<std::vector<std::string>, std::string>
    get_versions_satisfy_interval(const package_t& package) {
        // TODO: (`>1.2 and <=1.3.2` -> NG，`>1.2.0-alpha and <=1.3.2` -> OK)
        // `2.0.0` specific version or `>=0.1.2 and <3.4.0` version interval
        const semver::Interval i(get_interval(package));
        const std::vector<std::string> satisfied_versions =
            MITAMA_TRY(util::net::api::versions(get_name(package)))
            | boost::adaptors::filtered(
                [&i](std::string_view s){ return i.satisfies(s); }
            )
            | util::meta::containerized;

        if (satisfied_versions.empty()) {
            return mitama::failure(
                fmt::format(
                    "`{}: {}` not found; seem dependencies are broken",
                    get_name(package), get_interval(package)
                )
            );
        }
        return mitama::success(satisfied_versions);
    }

    using interval_cache_t =
        std::vector<
            std::tuple<
                package_t,
                std::vector<std::string> // versions in the interval
            >>;

    inline const package_t&
    get_package(const interval_cache_t::value_type& cache) noexcept {
        return std::get<0>(cache);
    }

    inline const std::vector<std::string>&
    get_versions(const interval_cache_t::value_type& cache) noexcept {
        return std::get<1>(cache);
    }

    inline bool
    exist_cache_impl(const package_t& a, const package_t& b) noexcept {
        return get_name(a) == get_name(b) &&
               get_version(a) == get_version(b);
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
            new_deps.emplace_back(package, std::nullopt);
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

    [[nodiscard]] mitama::result<duplicate_deps_t<with_deps>, std::string>
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
            const std::vector<std::string> versions =
                MITAMA_TRY(get_versions_satisfy_interval(package));
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
        return mitama::success(duplicate_deps);
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_RESOLVE_HPP
