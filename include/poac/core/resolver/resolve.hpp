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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/dynamic_bitset.hpp>
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
        std::string version; // TODO: semver::Version
        std::optional<std::unordered_map<std::string, std::string>> dependencies;

        // std::unordered_map::operator[] needs default constructor.
        Package()
            : version("")
            , dependencies(std::nullopt)
        {}

        Package(
            const std::string& version,
            std::optional<std::unordered_map<std::string, std::string>> dependencies
        )
            : version(version)
            , dependencies(dependencies)
        {}

        explicit Package(const std::string& version)
            : version(version)
            , dependencies(std::nullopt)
        {}

        ~Package() = default;
        Package(const Package&) = default;
        Package& operator=(const Package&) = default;
        Package(Package&&) noexcept = default;
        Package& operator=(Package&&) noexcept = default;
    };

    using DuplicateDeps = std::vector<std::pair<std::string, Package>>;
    using NoDuplicateDeps = std::unordered_map<std::string, Package>;

    struct ResolvedDeps {
        // Dependency information after activate.
        // Use for lock file and it is a state including another version of the same package.
        DuplicateDeps duplicate_deps;
        // Dependency information after backtrack.
        // Use for fetch packages.
        // Exclude them because you can not install different versions of packages with the same name.
        NoDuplicateDeps no_duplicate_deps;
    };

    template <typename T>
    std::string
    to_bin_str(T n, const std::size_t& digit_num) {
        std::string str;
        while (n > 0) {
            str.push_back('0' + (n & 1));
            n >>= 1;
        }
        str.resize(digit_num, '0');
        std::reverse(str.begin(), str.end());
        return str;
    }

    // A ∨ B ∨ C
    // A ∨ ¬B ∨ ¬C
    // ¬A ∨ B ∨ ¬C
    // ¬A ∨ ¬B ∨ C
    // ¬A ∨ ¬B ∨ ¬C
    void multiple_versions_cnf(const std::vector<int>& clause, std::vector<std::vector<int>>& clauses) {
        const int combinations = 1 << clause.size();
        for (int i = 0; i < combinations; ++i) { // index sequence
            boost::dynamic_bitset<> bs(to_bin_str(i, clause.size()));
            if (bs.count() == 1) {
                continue;
            }

            std::vector<int> new_clause;
            for (std::size_t j = 0; j < bs.size(); ++j) {
                if (bs[j]) {
                    new_clause.emplace_back(clause[j] * -1);
                } else {
                    new_clause.emplace_back(clause[j]);
                }
            }
            clauses.emplace_back(new_clause);
        }
    }

    bool check_already_added(const std::vector<int>& already_added, const int i) {
        auto last = already_added.cend();
        return std::find(already_added.cbegin(), last, i + 1) != last;
    }

    std::vector<std::vector<int>>
    create_cnf(const DuplicateDeps& activated) {
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

    ResolvedDeps solve_sat(const DuplicateDeps& activated, const std::vector<std::vector<int>>& clauses) {
        ResolvedDeps resolved_deps{};
        // deps.activated.size() == variables
        const auto [result, assignments] = sat::solve(clauses, activated.size());
        if (result == sat::Sat::completed) {
            PLOG_DEBUG << "SAT";
            for (const auto& a : assignments) {
                PLOG_DEBUG << a << " ";
                if (a > 0) {
                    const auto dep = activated[a - 1];
                    resolved_deps.duplicate_deps.push_back(dep);
                    resolved_deps.no_duplicate_deps[dep.first] = dep.second;
                }
            }
            PLOG_DEBUG << 0;
        }
        else {
            throw except::error("Could not solve in this dependencies.");
        }
        return resolved_deps;
    }

    ResolvedDeps backtrack_loop(const DuplicateDeps& activated) {
        const auto clauses = create_cnf(activated);
        // debug
        for (const auto& c : clauses) {
            for (const auto& l : c) {
                int index;
                if (l > 0) {
                    index = l - 1;
                } else {
                    index = (l * -1) - 1;
                }
                const auto [name, package] = activated[index];
                PLOG_DEBUG << fmt::format("{}-{}: {}, ", name, package.version, l);
            }
            PLOG_DEBUG << "";
        }
        return solve_sat(activated, clauses);
    }


    ResolvedDeps activated_to_backtracked(const ResolvedDeps& activated_deps) {
        ResolvedDeps resolved_deps;
        resolved_deps.duplicate_deps = activated_deps.duplicate_deps;
        for (const auto& [name, package] : activated_deps.duplicate_deps) {
            resolved_deps.no_duplicate_deps[name] = package;
        }
        return resolved_deps;
    }


    template <class SinglePassRange>
    bool duplicate_loose(const SinglePassRange& rng) { // If the same name
        const auto first = std::begin(rng);
        const auto last = std::end(rng);
        for (const auto& r : rng) {
            if (std::count_if(first, last, [&](const auto& x) {
                return x.first == r.first;
            }) > 1) {
                return true;
            }
        }
        return false;
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

    using IntervalCache = std::vector<std::tuple<std::string, std::string, std::vector<std::string>>>;
    constexpr std::size_t name_index = 0;
    constexpr std::size_t interval_index = 1;
    constexpr std::size_t versions_index = 2;

    // BFS activator
    void activate(
            const std::string& name,
            const std::string& version,
            DuplicateDeps& new_deps,
            IntervalCache& interval_cache)
    {
        // Check if root package resolved dependency (by version), and Check circulating
        if (std::find_if(
                new_deps.cbegin(),
                new_deps.cend(),
                [&](auto d) {
                    return d.first == name &&
                           d.second.version == version;
                }
            ) != new_deps.cend())
        {
            return;
        }

        // Get dependencies of dependencies
        const auto deps_api_res = io::net::api::deps(name, version).unwrap();
        if (deps_api_res.empty()) {
            new_deps.emplace_back(name, Package{ version, std::nullopt });
            return;
        }

        DuplicateDeps cur_deps_deps;
        for (const auto& [dep_name, dep_interval] : deps_api_res) {
            // Check if node package is resolved dependency (by interval)
            const auto itr =
                std::find_if(
                    interval_cache.cbegin(), interval_cache.cend(),
                    [&n=dep_name, &i=dep_interval](auto d) {
                        return std::get<name_index>(d) == n &&
                               std::get<interval_index>(d) == i;
                    }
                );
            if (itr != interval_cache.cend()) {
                for (const auto& dep_version : std::get<versions_index>(*itr)) {
                    cur_deps_deps.emplace_back(dep_name, Package{ dep_version, std::nullopt });
                }
            } else {
                const auto dep_versions = get_versions_satisfy_interval(dep_name, dep_interval);
                // Cache interval and versions pair
                interval_cache.emplace_back(dep_name, dep_interval, dep_versions);
                for (const auto& dep_version : dep_versions) {
                    cur_deps_deps.emplace_back(dep_name, Package{ dep_version, std::nullopt });
                }
            }
        }
        for (const auto& [name, package] : cur_deps_deps) {
            activate(name, package.version, new_deps, interval_cache);
        }
    }

    ResolvedDeps activate_deps_loop(const NoDuplicateDeps& deps) {
        ResolvedDeps new_deps{};
        IntervalCache interval_cache;

        // Activate the root of dependencies
        for (const auto& [name, package] : deps) {
            // Check if root package is resolved dependency (by interval)
            for (const auto& [n, i, versions] : interval_cache) {
                static_cast<void>(versions);
                if (name == n && package.version == i) {
                    continue;
                }
            }

            // Get versions using interval
            const auto versions = get_versions_satisfy_interval(name, package.version);
            // Cache interval and versions pair
            interval_cache.push_back({ {name}, {package.version}, {versions} });
            for (const auto& version : versions) {
                activate(name, version, new_deps.duplicate_deps, interval_cache);
            }
        }
        return new_deps;
    }

    // Builds the list of all packages required to build the first argument.
    [[nodiscard]] mitama::result<ResolvedDeps, std::string>
    resolve(const NoDuplicateDeps& deps) noexcept {
        try {
            const ResolvedDeps activated_deps = activate_deps_loop(deps);
            // When all dependencies are one package and one version,
            //   backtrack is not needed.
            if (duplicate_loose(activated_deps.duplicate_deps)) {
                return mitama::success(
                    backtrack_loop(activated_deps.duplicate_deps)
                );
            } else {
                return mitama::success(
                    activated_to_backtracked(activated_deps)
                );
            }
        } catch (const core::except::error& e) {
            return mitama::failure(e.what());
        } catch (...) {
            return mitama::failure("resolving packages failed");
        }
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_RESOLVE_HPP
