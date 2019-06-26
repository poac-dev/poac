#ifndef POAC_CORE_DEPER_RESOLVE_HPP
#define POAC_CORE_DEPER_RESOLVE_HPP

#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <string_view>
#include <sstream>
#include <regex>
#include <utility>
#include <map>
#include <optional>
#include <algorithm>
#include <iterator>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/dynamic_bitset.hpp>

#include "sat.hpp"
#include "semver.hpp"
#include "../except.hpp"
#include "../name.hpp"
#include "../../io.hpp"
#include "../../config.hpp"
#include "../../util/types.hpp"


namespace poac::core::resolver::resolve {
    namespace cache {
        bool resolve(const std::string& package_name) {
            namespace path = io::path;
            const auto package_path = path::poac_cache_dir / package_name;
            return path::validate_dir(package_path);
        }
    }
    namespace current {
        bool resolve(const std::string& current_package_name) {
            namespace path = io::path;
            const auto package_path = path::current_deps_dir / current_package_name;
            return path::validate_dir(package_path);
        }
    }
    namespace github {
        std::string clone_command(const std::string& name, const std::string& tag) {
            return "git clone -q https://github.com/" + name + ".git -b " + tag;
        }
    }
    std::string archive_url(const std::string& name, const std::string& version) {
        using namespace std::string_literals;
        return POAC_ARCHIVE_API + "/"s + name::hyphen_to_slash(name) + "/" + version;
    }


    template <typename... Bases>
    struct Package : Bases... {};

    struct Name { std::string name; };
    struct Interval { std::string interval; };
    struct Version { std::string version; };
    struct Versions { std::vector<std::string> versions; };
    struct Source { std::string source; };
    struct InDeps { std::vector<Package<Name, Version, Source, InDeps>> deps; };

    using PackageAll = Package<Name, Version, Source, InDeps>;
    bool operator==(const PackageAll& lhs, const PackageAll& rhs) {
        return lhs.name == rhs.name
            && lhs.version == rhs.version
            && lhs.source == rhs.source;
    }
    using PackageMini = Package<Version, Source>;
    bool operator==(const PackageMini& lhs, const PackageMini& rhs) {
        return lhs.version == rhs.version
            && lhs.source == rhs.source;
    }

    using Deps = std::vector<Package<Name, Interval, Source>>;
    using Activated = std::vector<PackageAll>;
    using Backtracked = std::map<std::string, PackageMini>;

    struct Resolved {
        // Dependency information after activate.
        // Use for lock file and it is a state including another version of the same package.
        Activated activated;
        // Dependency information after backtrack.
        // Use for fetch packages.
        // Exclude them because you can not install different versions of packages with the same name.
        Backtracked backtracked;
    };



    template <typename T>
    inline std::string to_bin_str(T n, const std::size_t& digit_num) {
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
                }
                else {
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
    create_cnf(const Activated& activated) {
        std::vector<std::vector<int>> clauses;
        std::vector<int> already_added;

        auto first = std::cbegin(activated);
        auto last = std::cend(activated);
        for (int i = 0; i < static_cast<int>(activated.size()); ++i) {
            if (check_already_added(already_added, i)) {
                continue;
            }

            const auto name_lambda = [&](const auto& x){ return x.name == activated[i].name; };
            // 現在指すパッケージと同名の他のパッケージは存在しない
            if (const auto count = std::count_if(first, last, name_lambda); count == 1) {
                std::vector<int> clause;
                clause.emplace_back(i + 1);
                clauses.emplace_back(clause);

                // index ⇒ deps
                if (!activated[i].deps.empty()) {
                    clause[0] *= -1;
                    for (const auto& dep : activated[i].deps) {
                        // 必ず存在することが保証されている
                        clause.emplace_back(util::types::index_of(first, last, dep) + 1);
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
                    if (!found->deps.empty()) {
                        std::vector<int> new_clause;
                        new_clause.emplace_back(index);
                        for (const auto& dep : found->deps) {
                            // 必ず存在することが保証されている
                            new_clause.emplace_back(util::types::index_of(first, last, dep) + 1);
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

    Resolved solve_sat(const Activated& activated, const std::vector<std::vector<int>>& clauses) {
        Resolved resolved_deps{};
        // deps.activated.size() == variables
        const auto [result, assignments] = sat::solve(clauses, activated.size());
        if (result == sat::Sat::completed) {
            io::cli::debugln("SAT");
            for (const auto& a : assignments) {
                io::cli::debug(a, " ");
                if (a > 0) {
                    const auto dep = activated[a - 1];
                    resolved_deps.activated.push_back(dep);
                    resolved_deps.backtracked[dep.name] = { {dep.version}, {dep.source} };
                }
            }
            io::cli::debugln(0);
        }
        else {
            throw except::error("Could not solve in this dependencies.");
        }
        return resolved_deps;
    }

    Resolved backtrack_loop(const Activated& activated) {
        const auto clauses = create_cnf(activated);
        // debug
        for (const auto& c : clauses) {
            for (const auto& l : c) {
                int index;
                if (l > 0) {
                    index = l - 1;
                }
                else {
                    index = (l * -1) - 1;
                }
                const auto ac = activated[index];
                io::cli::debug(ac.name, "-", ac.version, ": ", l, ", ");
            }
            io::cli::debugln();
        }
        return solve_sat(activated, clauses);
    }


    Resolved activated_to_backtracked(const Resolved& activated_deps) {
        Resolved resolved_deps;
        resolved_deps.activated = activated_deps.activated;
        for (const auto& a : activated_deps.activated) {
            resolved_deps.backtracked[a.name] = { {a.version}, {a.source} };
        }
        return resolved_deps;
    }


    template <class SinglePassRange>
    bool duplicate_loose(const SinglePassRange& rng) { // If the same name
        const auto first = std::begin(rng);
        const auto last = std::end(rng);
        for (const auto& r : rng) {
            if (std::count_if(first, last, [&](const auto& x) {
                return x.name == r.name;
            }) > 1) {
                return true;
            }
        }
        return false;
    }

    std::pair<std::string, std::string>
    get_from_dep(const boost::property_tree::ptree& dep) {
        return { dep.get<std::string>("name"), dep.get<std::string>("version") };
    }


    // Interval to multiple versions
    // `>=0.1.2 and <3.4.0` -> 2.5.0
    // `latest` -> 2.5.0
    // name is boost/config, no boost-config
    std::vector<std::string>
    decide_versions(const std::string& name, const std::string& interval) {
//        io::cli::echo("[versions] ", name, ": ", interval);

        // TODO: (`>1.2 and <=1.3.2` -> NG，`>1.2.0-alpha and <=1.3.2` -> OK)
        if (const auto versions = io::net::api::versions(name)) {
            if (interval == "latest") {
                const auto latest = std::max_element((*versions).begin(), (*versions).end(),
                        [](auto a, auto b) { return semver::Version(a) > b; });
                return { *latest };
            }
            else { // `2.0.0` specific version or `>=0.1.2 and <3.4.0` version interval
                std::vector<std::string> res;
                semver::Interval i(name, interval);
                copy_if(versions->begin(), versions->end(), back_inserter(res),
                        [&](std::string s) { return i.satisfies(s); });
                if (res.empty()) {
                    throw except::error(except::msg::not_found("`" + name + ": " + interval + "`"));
                }
                else {
                    return res;
                }
            }
        }
        else {
            throw except::error(except::msg::not_found("`" + name + ": " + interval + "`"));
        }
    }

    // BFS activator
    void activate(
            const std::string& name,
            const std::string& version,
            Activated& new_deps,
            std::vector<Package<Name, Interval, Versions>>& interval_cache)
    {
        // Check if root package resolved dependency (by version), and Check circulating
        if (auto last = new_deps.cend(); std::find_if(new_deps.cbegin(), last,
                [&](auto d) { return d.name == name && d.version == version; }) != last) {
            return;
        }

        // Get dependency of dependency
        if (const auto current_deps = io::net::api::deps(name, version)) {
            Activated cur_deps_deps;

            for (const auto& current_dep : *current_deps) {
                const auto [dep_name, dep_interval] = get_from_dep(current_dep.second);

                // Check if node package is resolved dependency (by interval)
                auto last = interval_cache.cend();
                const auto itr = std::find_if(interval_cache.cbegin(), last,
                        [&n=dep_name, &i=dep_interval](auto d) { return d.name == n && d.interval == i; });
                if (itr != last) {
                    for (const auto& dep_version : itr->versions) {
                        cur_deps_deps.push_back({ {dep_name}, {dep_version}, {"poac"}, {} });
                    }
                }
                else {
                    const auto dep_versions = decide_versions(dep_name, dep_interval);
                    // Cache interval and versions pair
                    interval_cache.push_back({ {dep_name}, {dep_interval}, {dep_versions} });
                    for (const auto& dep_version : dep_versions) {
                        cur_deps_deps.push_back({ {dep_name}, {dep_version}, {"poac"}, {} });
                    }
                }
            }
            new_deps.push_back({ {name}, {version}, {"poac"}, {cur_deps_deps} });
            for (const auto& cur_dep : cur_deps_deps) {
                activate(cur_dep.name, cur_dep.version, new_deps, interval_cache);
            }
        }
        else {
            new_deps.push_back({ {name}, {version}, {"poac"}, {} });
        }
    }

    Resolved activate_deps_loop(const Deps& deps) {
        Resolved new_deps{};
        std::vector<Package<Name, Interval, Versions>> interval_cache;

        // Activate the root of dependencies
        for (const auto& dep : deps) {
            // Check if root package is resolved dependency (by interval)
            if (auto last = interval_cache.cend(); std::find_if(interval_cache.cbegin(), last,
                    [&](auto d) { return d.name == dep.name && d.interval == dep.interval; }) != last)
            { continue; }

            // Get versions using interval
            const auto versions = decide_versions(dep.name, dep.interval);
            // Cache interval and versions pair
            interval_cache.push_back({ {dep.name}, {dep.interval}, {versions} });
            for (const auto& version : versions) {
                activate(dep.name, version, new_deps.activated, interval_cache);
            }
        }
        return new_deps;
    }

    // Builds the list of all packages required to build the first argument.
    Resolved resolve(const Deps& deps) {
        Deps poac_deps;
        Deps others_deps;

        // Divide poac and others only when src is poac, solve dependency.
        for (const auto& dep : deps) {
            if (dep.source == "poac") {
                poac_deps.emplace_back(dep);
            }
            else {
                others_deps.emplace_back(dep);
            }
        }

        const Resolved activated_deps = activate_deps_loop(poac_deps);
        Resolved resolved_deps;
        // 全ての依存関係が一つのパッケージ，一つのバージョンに依存する時はbacktrackが不要
        if (duplicate_loose(activated_deps.activated)) {
            resolved_deps = backtrack_loop(activated_deps.activated);
        }
        else {
            resolved_deps = activated_to_backtracked(activated_deps);
        }

        // Merge others_deps into resolved_deps
        for (const auto& dep : others_deps) {
            resolved_deps.activated.push_back({ {dep.name}, {dep.interval}, {dep.source}, {} });
            resolved_deps.backtracked[dep.name] = { {dep.interval}, {dep.source} };
        }
        return resolved_deps;
    }
} // end namespace
#endif // !POAC_CORE_DEPER_RESOLVE_HPP
