#ifndef POAC_CORE_RESOLVER_RESOLVE_HPP
#define POAC_CORE_RESOLVER_RESOLVE_HPP

// std
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
#include <boost/range/algorithm/find_if.hpp>
#include <fmt/core.h>
#include <mitama/result/result.hpp>
#include <plog/Log.h>

// internal
#include <poac/config.hpp>
#include <poac/core/except.hpp>
#include <poac/core/name.hpp>
#include <poac/core/resolver/sat.hpp>
#include <poac/io/net.hpp>
#include <poac/io/path.hpp>
#include <poac/util/meta.hpp>
#include <poac/util/semver/semver.hpp>

namespace poac::core::resolver::resolve {
    struct Package {
        // A package depends packages have many different versions of one name,
        //   so this should not be std::unordered_map.
        // Also, package information does not need
        //   package's dependency's dependencies,
        //   so second value of std::pair is not Package (this type)
        //   just needs std::string indicated specific version.
        using package_deps_t =
            std::optional<std::vector<std::pair<std::string, std::string>>>;

        std::string version; // TODO: semver::Version
        package_deps_t dependencies;

        // std::unordered_map::operator[] needs default constructor.
        Package() : version(""), dependencies(std::nullopt) {}

        Package(const std::string& version, package_deps_t dependencies)
            : version(version), dependencies(dependencies) {}

        explicit Package(const std::string& version)
            : version(version), dependencies(std::nullopt) {}

        ~Package() = default;
        Package(const Package&) = default;
        Package& operator=(const Package&) = default;
        Package(Package&&) noexcept = default;
        Package& operator=(Package&&) noexcept = default;
    };

    using duplicate_deps_t = std::vector<std::pair<std::string, Package>>;
    using unique_deps_t = std::unordered_map<std::string, Package>;

    std::string to_binary_numbers(const int& x, const std::size_t& digit) {
        return fmt::format(FMT_STRING("{:0{}b}"), x, digit);
    }

    // A ∨ B ∨ C
    // A ∨ ¬B ∨ ¬C
    // ¬A ∨ B ∨ ¬C
    // ¬A ∨ ¬B ∨ C
    // ¬A ∨ ¬B ∨ ¬C
    void multiple_versions_cnf(const std::vector<int>& clause, std::vector<std::vector<int>>& clauses) {
        const int combinations = 1 << clause.size();
        for (int i = 0; i < combinations; ++i) { // index sequence
            boost::dynamic_bitset<> bs(to_binary_numbers(i, clause.size()));
            if (bs.count() == 1) {
                continue;
            }

            std::vector<int> new_clause;
            for (std::size_t j = 0; j < bs.size(); ++j) {
                new_clause.emplace_back(bs[j] ? clause[j] * -1 : clause[j]);
            }
            clauses.emplace_back(new_clause);
        }
    }

    bool check_already_added(const std::vector<int>& already_added, const int i) {
        auto last = already_added.cend();
        return std::find(already_added.cbegin(), last, i + 1) != last;
    }

    std::vector<std::vector<int>>
    create_cnf(const duplicate_deps_t& activated) {
        std::vector<std::vector<int>> clauses;
        std::vector<int> already_added;

        auto first = std::cbegin(activated);
        auto last = std::cend(activated);
        for (int i = 0; i < static_cast<int>(activated.size()); ++i) {
            if (check_already_added(already_added, i)) {
                continue;
            }

            const auto name_lambda = [&](const auto& x){ return x.first == activated[i].first; };
            // 現在指すパッケージと同名の他のパッケージは存在しない
            if (const auto count = std::count_if(first, last, name_lambda); count == 1) {
                std::vector<int> clause;
                clause.emplace_back(i + 1);
                clauses.emplace_back(clause);

                // index ⇒ deps
                if (!activated[i].second.dependencies.has_value()) {
                    clause[0] *= -1;
                    for (const auto& [name, version] : activated[i].second.dependencies.value()) {
                        // 必ず存在することが保証されている
                        clause.emplace_back(util::meta::index_of_if(first, last, [&n=name, &v=version](auto d){ return d.first == n && d.second.version == v; }) + 1);
                    }
                    clauses.emplace_back(clause);
                }
            }
            else if (count > 1) {
                std::vector<int> clause;

                for (auto found = first; found != last; found = std::find_if(found, last, name_lambda)) {
                    const auto index = std::distance(first, found);
                    clause.emplace_back(index + 1);
                    already_added.emplace_back(index + 1);

                    // index ⇒ deps
                    if (!found->second.dependencies.has_value()) {
                        std::vector<int> new_clause;
                        new_clause.emplace_back(index);
                        for (const auto& [name, version] : found->second.dependencies.value()) {
                            // 必ず存在することが保証されている
                            new_clause.emplace_back(util::meta::index_of_if(first, last, [&n=name, &v=version](auto d){ return d.first == n && d.second.version == v; }) + 1);
                        }
                        clauses.emplace_back(new_clause);
                    }
                    ++found;
                }
                multiple_versions_cnf(clause, clauses);
            }
        }
        return clauses;
    }

    unique_deps_t
    solve_sat(const duplicate_deps_t& activated, const std::vector<std::vector<int>>& clauses) {
        unique_deps_t resolved_deps{};
        // deps.activated.size() == variables
        const auto [result, assignments] = sat::solve(clauses, activated.size());
        if (result == sat::Sat::completed) {
            PLOG_DEBUG << "SAT";
            for (const auto& a : assignments) {
                PLOG_DEBUG << a << " ";
                if (a > 0) {
                    const auto dep = activated[a - 1];
                    resolved_deps[dep.first] = dep.second;
                }
            }
            PLOG_DEBUG << 0;
        } else {
            throw except::error("Could not solve in this dependencies.");
        }
        return resolved_deps;
    }

    unique_deps_t
    backtrack_loop(const duplicate_deps_t& activated) {
        const auto clauses = create_cnf(activated);
        IF_PLOG(plog::debug) {
            for (const auto& c : clauses) {
                for (const auto& l : c) {
                    const auto& [name, package] =
                        activated[l > 0 ? l - 1 : (l * -1) - 1];
                    PLOG_DEBUG <<
                        fmt::format(
                            "{}-{}: {}, ",
                            name, package.version, l
                        );
                }
                PLOG_DEBUG << "";
            }
        }
        return solve_sat(activated, clauses);
    }

    unique_deps_t
    activated_to_backtracked(const duplicate_deps_t& activated_deps) {
        unique_deps_t resolved_deps;
        for (const auto& [name, package] : activated_deps) {
            resolved_deps[name] = package;
        }
        return resolved_deps;
    }

    template <class SinglePassRange>
    bool duplicate_loose(const SinglePassRange& rng) {
        const auto first = std::begin(rng);
        const auto last = std::end(rng);
        return std::find_if(first, last, [&](const auto& x){
            return std::count_if(first, last, [&](const auto& y) {
              return x.first == y.first;
            }) > 1;
        }) != last;
    }

    // Interval to multiple versions
    // `>=0.1.2 and <3.4.0` -> { 2.4.0, 2.5.0 }
    // `latest` -> { 2.5.0 }
    // name is boost/config, no boost-config
    std::vector<std::string>
    get_versions_satisfy_interval(const std::string& name, const std::string& interval) {
        // TODO: (`>1.2 and <=1.3.2` -> NG，`>1.2.0-alpha and <=1.3.2` -> OK)
        const std::vector<std::string> versions =
            io::net::api::versions(name).unwrap();
        if (interval == "latest") {
            return {
                *std::max_element(
                    versions.cbegin(),
                    versions.cend(),
                    [](auto a, auto b){
                        return semver::Version(a) > b;
                    }
                )
            };
        } else { // `2.0.0` specific version or `>=0.1.2 and <3.4.0` version interval
            std::vector<std::string> res;
            semver::Interval i(name, interval);
            std::copy_if(
                versions.cbegin(),
                versions.cend(),
                back_inserter(res),
                [&](std::string s) { return i.satisfies(s); }
            );
            if (res.empty()) {
                throw except::error(
                    fmt::format(
                        "`{}: {}` not found. seem dependencies are broken",
                        name, interval
                    )
                );
            } else {
                return res;
            }
        }
    }

    using interval_cache_t =
        std::vector<
            std::tuple<
                std::string, // name
                std::string, // interval
                std::vector<std::string> // versions in the interval
            >>;

    Package::package_deps_t::value_type
    gather_deps_of_deps(
        const std::unordered_map<std::string, std::string>& deps_api_res,
        interval_cache_t& interval_cache)
    {
        Package::package_deps_t::value_type cur_deps_deps;
        for (const auto& [dep_name, dep_interval] : deps_api_res) {
            // Check if node package is resolved dependency (by interval)
            const auto itr =
                boost::range::find_if(
                    interval_cache,
                    [&n=dep_name, &i=dep_interval](const auto& d) {
                      return std::get<0>(d) == n &&
                             std::get<1>(d) == i;
                    }
                );
            if (itr != interval_cache.cend()) { // cache found
                for (const auto& dep_version : std::get<2>(*itr)) {
                    cur_deps_deps.emplace_back(dep_name, dep_version);
                }
            } else {
                const auto dep_versions =
                    get_versions_satisfy_interval(dep_name, dep_interval);
                // Cache interval and versions pair
                interval_cache.emplace_back(dep_name, dep_interval, dep_versions);
                for (const auto& dep_version : dep_versions) {
                    cur_deps_deps.emplace_back(dep_name, dep_version);
                }
            }
        }
        return cur_deps_deps;
    }

    void gather_deps(
        const std::string& name,
        const std::string& version,
        duplicate_deps_t& new_deps,
        interval_cache_t& interval_cache)
    {
        // Check if root package resolved dependency
        //   (whether the specific version is the same),
        //   and check circulating
        if (util::meta::find_if(new_deps, [&](const auto& d) {
          return d.first == name && d.second.version == version;
        })) {
            return;
        }

        // Get dependencies of dependencies
        const auto deps_api_res = io::net::api::deps(name, version).unwrap();
        if (deps_api_res.empty()) {
            new_deps.emplace_back(name, Package{ version, std::nullopt });
        } else {
            const auto deps_of_deps = gather_deps_of_deps(deps_api_res, interval_cache);

            // Store dependency and the dependency's dependencies.
            new_deps.emplace_back(name, Package{ version, deps_of_deps });

            // Gather dependencies of dependencies of dependencies. lol
            for (const auto& [name, version] : deps_of_deps) {
                gather_deps(name, version, new_deps, interval_cache);
            }
        }
    }

    [[nodiscard]] mitama::result<duplicate_deps_t, std::string>
    gather_all_deps(const unique_deps_t& deps) {
        duplicate_deps_t duplicate_deps{};
        interval_cache_t interval_cache{};

        // Activate the root of dependencies
        for (const auto& [name, package] : deps) {
            // Check whether the packages specified in poac.toml
            //   are already resolved which includes
            //   that package's dependencies and package's versions
            //   by checking whether package's interval is the same.
            if (util::meta::find_if(
                    interval_cache,
                    [&name=name, &package=package](const auto& cache){
                        return name == std::get<0>(cache) &&
                            package.version == std::get<1>(cache);
                    }
                )) {
                continue;
            }

            // Get versions using interval
            // FIXME: versions API and deps API are received the almost same responses
            const auto versions = get_versions_satisfy_interval(name, package.version);
            // Cache interval and versions pair
            interval_cache.emplace_back(name, package.version, versions);
            for (const auto& version : versions) {
                gather_deps(name, version, duplicate_deps, interval_cache);
            }
        }
        return mitama::success(duplicate_deps);
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_RESOLVE_HPP
