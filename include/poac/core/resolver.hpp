#ifndef POAC_CORE_RESOLVER_HPP
#define POAC_CORE_RESOLVER_HPP

#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <sstream>
#include <regex>
#include <utility>
#include <map>
#include <optional>
#include <algorithm> // copy_if
#include <iterator> // back_inserter

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/dynamic_bitset.hpp>

#include "exception.hpp"
#include "naming.hpp"
#include "sat.hpp"
#include "semver.hpp"
#include "../io/file.hpp"
#include "../io/network.hpp"
#include "../config.hpp"
#include "../util/types.hpp"


namespace poac::core::resolver {
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
        std::string archive_url(const std::string& name, const std::string& tag) {
            return "/" + name + "/archive/" + tag + ".tar.gz";
        }
    }

    std::string archive_url(const std::string& name, const std::string& version) {
        return "/poac-pm.appspot.com/" + core::naming::to_cache("poac", name, version) + ".tar.gz";
    }

    // Interval to multiple versions
    // `>=0.1.2 and <3.4.0` -> 2.5.0
    // name is boost/config, no boost-config
    std::vector<std::string>
    decide_versions(const std::string& name, const std::string& interval) {
        // TODO: (`>1.2 and <=1.3.2` -> NG，`>1.2.0-alpha and <=1.3.2` -> OK)
        boost::property_tree::ptree pt;
        {
            std::stringstream ss;
            ss << io::network::get(POAC_PACKAGES_API + name + "/versions");
            boost::property_tree::json_parser::read_json(ss, pt);
        }
        const auto versions = util::types::ptree_to_vector<std::string>(pt);

        std::vector<std::string> res;
        semver::Interval i(name, interval);
        copy_if(versions.begin(), versions.end(), back_inserter(res),
                [&](std::string s){ return i.satisfies(s); });
        if (res.empty()) {
            throw exception::error("`" + name + ": " + interval + "` was not found.");
        }
        else {
            return res;
        }
    }


    struct Package {
        std::string name;
        std::string version;
        std::string source;
        std::vector<Package> deps;
    };
    bool operator==(const Package& lhs, const Package& rhs) {
        return lhs.name == rhs.name
            && lhs.version == rhs.version
            && lhs.source == rhs.source;
    }
    struct MiniPackage {
        std::string version;
        std::string source;
    };
    bool operator==(const MiniPackage& lhs, const MiniPackage& rhs) {
        return lhs.version == rhs.version
            && lhs.source == rhs.source;
    }

    using Activated = std::vector<Package>;
    using Backtracked = std::map<std::string, MiniPackage>;
    struct Resolved {
        // activate後dependency情報，lockファイルに必要．
        // 同一パッケージの別バージョンを含む状態．
        // これを使用して充足可能性問題を解き，bactrackedに代入する．
        Activated activated;
        // backtrack後は，同じパッケージで異なるバージョンなどを同時にはむりなので，排除している．即ちパッケージ名は一意である．
        // backtrack後のdependency情報. installに必要．installには，依存の依存は情報としては必要無い．
        Backtracked backtracked;
    };

    struct Dep {
        std::string name;
        std::string version;
        std::string source;
    };
    using Deps = std::vector<Dep>;


    template <class SinglePassRange, class T>
    std::optional<std::size_t>
    indexof(const SinglePassRange& rng, const T& t) {
        const auto first = std::begin(rng);
        const auto last = std::end(rng);
        auto result = std::find(first, last, t);
        if (result == last) {
            return std::nullopt;
        }
        else {
            return std::distance(first, result);
        }
    }

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


    Resolved backtrack_loop(const Resolved& deps) {
        std::vector<std::vector<int>> clauses;
        std::vector<int> already_added;

        const auto first = std::begin(deps.activated);
        const auto last = std::end(deps.activated);
        for(auto itr = first; itr != last; ++itr) {
            if (std::find(already_added.begin(), already_added.end(), std::distance(first, itr) + 1) != already_added.end()) {
                continue;
            }

            long c = std::count_if(first, last, [&](const auto& x){ return x.name == itr->name; });
            if (c == 1) { // 現在指すパッケージと同名の他のパッケージは存在しない
                std::vector<int> clause;
                clause.emplace_back(std::distance(first, itr) + 1);
                clauses.emplace_back(clause);

                if (!itr->deps.empty()) {
                    clause[0] *= -1; // index ⇒ deps
                    for (const auto& dep : itr->deps) {
                        // 必ず存在することが保証されている
                        long index = std::distance(first, std::find(first, last, dep)) + 1;
                        clause.emplace_back(index);
                    }
                    clauses.emplace_back(clause);
                }
            }
            else if (c > 1) {
                std::vector<int> clause;

                auto found = std::begin(deps.activated);
                while (true) {
                    found = std::find_if(found, last, [&](const auto& x){ return x.name == itr->name; }); // FIXME: 同じ名前が複数存在する時に，二回以上繰り返されてしまう．
                    if (found == last) {
                        break;
                    }
                    else {
                        long index = std::distance(first, found) + 1;
                        clause.emplace_back(index);
                        already_added.emplace_back(index);

                        // index ⇒ deps
                        if (!found->deps.empty()) {
                            std::vector<int> new_clause;
                            new_clause.emplace_back(index);
                            for (const auto& dep : found->deps) {
                                // 必ず存在することが保証されている
                                long index2 = std::distance(first, std::find(first, last, dep)) + 1;
                                new_clause.emplace_back(index2);
                            }
                            clauses.emplace_back(new_clause);
                        }
                    }
                    ++found;
                }

                // A ∨ B ∨ C
                // A ∨ ¬B ∨ ¬C
                // ¬A ∨ B ∨ ¬C
                // ¬A ∨ ¬B ∨ C
                // ¬A ∨ ¬B ∨ ¬C
                const int combinations = 1 << clause.size();
                for (int i = 0; i < combinations; ++i) {
                    boost::dynamic_bitset<> bs(to_bin_str(i, clause.size()));
                    if (bs.count() == 1) {
                        continue;
                    }

                    std::vector<int> new_clause;
                    for (std::size_t i = 0; i < bs.size(); ++i) {
                        if (bs[i] == 0) {
                            new_clause.push_back(clause[i]);
                        }
                        else {
                            new_clause.push_back(clause[i] * -1);
                        }
                    }
                    clauses.emplace_back(new_clause);
                }
            }
        }


        for (const auto& cls : clauses) {
            for (const auto& literal : cls) {
                int index;
                if (literal > 0) {
                    index = literal - 1;

                }
                else {
                    index = (literal * -1) - 1;
                }
                std::cout << deps.activated[index].name << "-" << deps.activated[index].version << ": " << literal << ", ";
            }
            std::cout << std::endl;
        }


        Resolved resolved_deps;

        // deps.activated.size() == variables
        sat::Formula formula(clauses, deps.activated.size());
        const auto [result, assignments] = sat::solve(formula);
        if (result == sat::Sat::completed) {
            for (const auto& a : assignments) {
                if (a > 0) {
                    const auto dep = deps.activated[a];
                    resolved_deps.activated.push_back(dep);
                    resolved_deps.backtracked[dep.name] = { dep.version, dep.source };
                }
            }
        }
        else {
            throw exception::error("Could not solve in this dependencies.");
        }

        return resolved_deps;
    }


    Resolved activated_to_backtracked(const Resolved& activated_deps) {
        Resolved resolved_deps;
        resolved_deps.activated = activated_deps.activated;
        for (const auto& a : activated_deps.activated) {
            resolved_deps.backtracked[a.name] = { a.version, a.source };
        }
        return resolved_deps;
    }


    // Check if it has duplicate elements.
    template <class SinglePassRange>
    bool duplicate(const SinglePassRange& rng) {
        const auto first = std::begin(rng);
        const auto last = std::end(rng);
        for (const auto& r : rng) {
            int c = std::count(first, last, r);
            if (c > 1) {
                return true;
            }
        }
        return false;
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
        return make_pair(name, interval);
    }

    std::optional<boost::property_tree::ptree>
    get_deps_api(const std::string& name, const std::string& version) {
        std::stringstream ss;
        ss << io::network::get(POAC_PACKAGES_API + name + "/" + version + "/deps");
        if (ss.str() == "null") {
            return std::nullopt;
        }
        else {
            boost::property_tree::ptree pt;
            boost::property_tree::json_parser::read_json(ss, pt);
            return pt;
        }
    }

    void activate(Resolved& new_deps,
                  Activated prev_activated_deps,
                  Activated& activated_deps,
                  const std::string& name,
                  const std::string& interval)
    {
        for (const auto& version : resolver::decide_versions(name, interval)) {
            // {}なのは，依存先の依存先までは必要無く，依存先で十分なため
            activated_deps.push_back({ name, version, "poac", {} });

            const auto lambda = [name=name, version=version](auto d) {
                return d.name == name && d.version == version;
            };
            // Circulating
            auto result = std::find_if(prev_activated_deps.begin(), prev_activated_deps.end(), lambda);
            if (result != prev_activated_deps.end()) {
                break;
            }
            // Resolved
            auto result2 = std::find_if(new_deps.activated.begin(), new_deps.activated.end(), lambda);
            if (result2 !=
                new_deps.activated.end()) {
                break;
            }

            prev_activated_deps.push_back({ name, version, "poac", {} });

            if (const auto current_deps = get_deps_api(name, version)) {
                Activated activated_deps2;

                for (const auto& current_dep : *current_deps) {
                    const auto[dep_name, dep_interval] = get_from_dep(current_dep.second);
                    activate(new_deps, prev_activated_deps, activated_deps2, dep_name, dep_interval);
                }
                new_deps.activated.push_back({ name, version, "poac", activated_deps2 });
            }
            else {
                new_deps.activated.push_back({ name, version, "poac", {} });
            }
        }
    }

    Resolved activate_deps_loop(const Deps& deps) {
        Resolved new_deps;
        for (const auto& dep : deps) {
            // Activate the top of dependencies
            for (const auto& version : resolver::decide_versions(dep.name, dep.version)) {
                if (const auto current_deps = get_deps_api(dep.name, version)) {
                    Activated activated_deps;

                    for (const auto& current_dep : *current_deps) {
                        const auto [dep_name, dep_interval] = get_from_dep(current_dep.second);

                        Activated prev_activated_deps;
                        prev_activated_deps.push_back({ dep.name, version, "poac", {} }); // push top
                        activate(new_deps, prev_activated_deps, activated_deps, dep_name, dep_interval);
                    }
                    new_deps.activated.push_back({ dep.name, version, "poac", activated_deps });
                }
                else {
                    new_deps.activated.push_back({ dep.name, version, "poac", {} });
                }
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

        // 木の末端からpush_backされていくため，依存が無いものが一番最初の要素になる．
        // つまり，配列のループのそのままの順番でインストールやビルドを行うと不具合は起きない
        const Resolved activated_deps = activate_deps_loop(poac_deps);
        Resolved resolved_deps;
        // 全ての依存関係が一つのパッケージ，一つのバージョンに依存する時はそのままインストールできる．
        if (duplicate_loose(activated_deps.activated)) {
            resolved_deps = backtrack_loop(activated_deps);
        }
        else {
            resolved_deps = activated_to_backtracked(activated_deps);
        }

        // Merge others_deps into resolved_deps
        for (const auto& dep : others_deps) {
            resolved_deps.activated.push_back({ dep.name, dep.version, dep.source, {} });
            resolved_deps.backtracked[dep.name] = { dep.version, dep.source };
        }
        return resolved_deps;
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_HPP
