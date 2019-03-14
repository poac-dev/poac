#ifndef POAC_CORE_DEPER_RESOLVER_HPP
#define POAC_CORE_DEPER_RESOLVER_HPP

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
#include "../exception.hpp"
#include "../naming.hpp"
#include "../../io/file.hpp"
#include "../../io/net.hpp"
#include "../../config.hpp"
#include "../../util/types.hpp"


namespace poac::core::deper::resolver {
    namespace cache {
        bool resolve(const std::string& package_name) {
            namespace path = io::file::path;
            const auto package_path = path::poac_cache_dir / package_name;
            return path::validate_dir(package_path);
        }
    }
    namespace current {
        bool resolve(const std::string& current_package_name) {
            namespace path = io::file::path;
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
        return POAC_ARCHIVE_API + "/"s + naming::hyphen_to_slash(name) + "/" + version;
    }


    template <typename... Bases>
    struct Package : Bases... {};

    struct Name { std::string name; };
    struct Interval { std::string interval; };
    struct Version { std::string version; };
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
        for (int u = 0; u < combinations; ++u) { // index sequence
            boost::dynamic_bitset<> bs(to_bin_str(u, clause.size()));
            if (bs.count() == 1) {
                continue;
            }

            std::vector<int> new_clause;
            for (std::size_t j = 0; j < bs.size(); ++j) {
                if (bs[j] == 0) {
                    new_clause.push_back(clause[j]);
                }
                else {
                    new_clause.push_back(clause[j] * -1);
                }
            }
            clauses.emplace_back(new_clause);
        }
    }

    std::vector<std::vector<int>>
    create_cnf(const Activated& activated) {
        std::vector<std::vector<int>> clauses;
        std::vector<int> already_added;

        const auto first = std::begin(activated);
        const auto last = std::end(activated);
        for (int i = 0; i < static_cast<int>(activated.size()); ++i) {
            if (std::find(already_added.begin(), already_added.end(), i + 1) != already_added.end()) {
                continue;
            }

            const auto name_lambda = [&](const auto& x){ return x.name == activated[i].name; };
            const auto count = std::count_if(first, last, name_lambda);
            if (count == 1) { // 現在指すパッケージと同名の他のパッケージは存在しない
                std::vector<int> clause;
                clause.emplace_back(i + 1);
                clauses.emplace_back(clause);

                // index ⇒ deps
                if (!activated[i].deps.empty()) {
                    clause[0] *= -1;
                    for (const auto& dep : activated[i].deps) {
                        // 必ず存在することが保証されている
                        const auto index = std::distance(first, std::find(first, last, dep));
                        clause.emplace_back(index + 1);
                    }
                    clauses.emplace_back(clause);
                }
            }
            else if (count > 1) {
                std::vector<int> clause;

                for (auto found = first; found != last; found = std::find_if(found, last, name_lambda)) {
                    const auto index = std::distance(first, found) + 1;
                    clause.emplace_back(index);
                    already_added.emplace_back(index);

                    // index ⇒ deps
                    if (!found->deps.empty()) {
                        std::vector<int> new_clause;
                        new_clause.emplace_back(index);
                        for (const auto& dep : found->deps) {
                            // 必ず存在することが保証されている
                            const auto index2 = std::distance(first, std::find(first, last, dep)) + 1;
                            new_clause.emplace_back(index2);
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
            throw exception::error("Could not solve in this dependencies.");
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
            long c = std::count_if(first, last, [&](const auto& x){ return x.name == r.name; });
            if (c > 1) {
                return true;
            }
        }
        return false;
    }

    std::pair<std::string, std::string>
    get_from_dep(const boost::property_tree::ptree& dep) {
        const auto name = dep.get<std::string>("name");
        const auto interval = dep.get<std::string>("version");
        return { name, interval };
    }


    // Interval to multiple versions
    // `>=0.1.2 and <3.4.0` -> 2.5.0
    // `latest` -> 2.5.0
    // name is boost/config, no boost-config
    std::vector<std::string>
    decide_versions(const std::string& name, const std::string& interval) {
//        io::cli::echo(name, ": ", interval);

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
                    throw exception::error(
                            exception::msg::not_found("`" + name + ": " + interval + "`"));
                }
                else {
                    return res;
                }
            }
        }
        else {
            throw exception::error(
                    exception::msg::not_found("`" + name + ": " + interval + "`"));
        }
    }

    //       ○
    //      / |
    //    _________________________
    //   | ○ ← name & interval   ○ | ← prev_deps_node
    //    ￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣￣
    //    /|  /| |
    //   ○ ○  ○ ○ ○
    void activate(
            const std::string& name,
            const std::string& interval,
            Activated& new_deps,
            Activated& all_deps_node,
            std::vector<Package<Name, Interval>>& cache_all_deps,
            Activated& prev_deps_node)
    {
        // Check resolved dependency (by interval)
        if (auto last = cache_all_deps.cend(); std::find_if(cache_all_deps.cbegin(), last, [&](auto d) {
            return d.name == name && d.interval == interval; }) != last)
        {   // It is not added to cur_deps_node (N-1) by being skipped.
            // Therefore, the problem of not being selected as a package dependency that occurs.
            // As a solving measure, use information already in new_deps.
            semver::Interval i(name, interval);
            auto nd_last = new_deps.cend();
            auto match_f = [&](auto d) { return d.name == name && i.satisfies(d.version); };
            auto itr = std::find_if(new_deps.cbegin(), nd_last, match_f);
            if (itr != nd_last) {
                while (itr != nd_last) {
                    prev_deps_node.push_back({ {itr->name}, {itr->version}, {"poac"}, {} });
                    itr = std::find_if(itr + 1, nd_last, match_f);
                }
            }
            // It is assumed that it is peer-dependency because it does not already exist in new_deps,
            //  although it is cached in cache_all_deps. So I call API again. (TODO: any solutions?)
            else { // FIXME: There is room for optimization. (Useless API calls)
                for (const auto& version : decide_versions(name, interval)) {
                    prev_deps_node.push_back({ {name}, {version}, {"poac"}, {} });
                }
            }

            return;
        }
        cache_all_deps.push_back({ {name}, {interval} }); // push top

        // Get versions using interval
        for (const auto& version : decide_versions(name, interval)) {
            prev_deps_node.push_back({ {name}, {version}, {"poac"}, {} });

            const auto match_f = [&](auto d) { return d.name == name && d.version == version; };
            // Check circulating
            if (auto last = all_deps_node.cend(); std::find_if(all_deps_node.cbegin(), last, match_f) != last) {
                continue;
            }
            // Check resolved dependency (by version)
            if (auto last = new_deps.cend(); std::find_if(new_deps.cbegin(), last, match_f) != last) {
                continue;
            }
            all_deps_node.push_back({ {name}, {version}, {"poac"}, {} }); // push top

            // Get deps of deps
            if (const auto current_deps = io::net::api::deps(name, version)) {
                Activated cur_deps_node;

                for (const auto& current_dep : *current_deps) {
                    const auto [dep_name, dep_interval] = get_from_dep(current_dep.second);
                    activate(dep_name, dep_interval, new_deps, all_deps_node, cache_all_deps, cur_deps_node);
                }
                new_deps.push_back({ {name}, {version}, {"poac"}, {cur_deps_node} });
            }
            else {
                new_deps.push_back({ {name}, {version}, {"poac"}, {} });
            }
        }
    }

    Resolved activate_deps_loop(const Deps& deps) {
        Resolved new_deps{};
        std::vector<Package<Name, Interval>> cache_all_deps;
        Activated all_deps_node;

        for (const auto& dep : deps) {
            // Activate the top of dependencies
            Activated cur_deps_node;
            activate(dep.name, dep.interval, new_deps.activated, all_deps_node, cache_all_deps, cur_deps_node);
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
#endif // !POAC_CORE_DEPER_RESOLVER_HPP
