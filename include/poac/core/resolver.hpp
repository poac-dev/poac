#ifndef POAC_CORE_RESOLVER_HPP
#define POAC_CORE_RESOLVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <utility>
#include <optional>
#include <algorithm> // copy_if
#include <iterator> // back_inserter

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>
#include <cvc4/cvc4.h>

#include "exception.hpp"
#include "naming.hpp"
#include "../io/file.hpp"
#include "../io/network.hpp"


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
            return "https://github.com/" + name + "/archive/" + tag + ".tar.gz";
        }
        std::string archive_url(const std::string& name) {
            return "https://github.com/" + name + ".git";
        }
    }

    namespace poac {
        std::string archive_url(const std::string& name, const std::string& version) {
            return "https://storage.googleapis.com/poac-pm.appspot.com/"
                   + core::naming::to_cache("poac", name, version) + ".tar.gz";
        }

        // boost::property_tree::ptree : {"key": ["array", "...", ...]}
        //  -> std::vector<T> : ["array", "...", ...]
        template <typename T, typename U, typename K=typename U::key_type>
        std::vector<T> as_vector(const U& pt, const K& key) { // as_vector(pt, "key")
            std::vector<T> r;
            for (const auto& item : pt.get_child(key)) {
                r.push_back(item.second.template get_value<T>());
            }
            return r;
        }
        // boost::property_tree::ptree : ["array", "...", ...]
        //  -> std::vector<T> : ["array", "...", ...]
        template <typename T, typename U>
        std::vector<T> as_vector(const U& pt) {
            std::vector<T> r;
            for (const auto& item : pt) {
                r.push_back(item.second.template get_value<T>());
            }
            return r;
        }

        // e.g. >0.1.3 and >=0.3.2, <0.1.3 and <0.3.2
        void is_wasteful_comparison_operation(
                const std::string& first_comp_op,
                const std::string& second_comp_op,
                const std::string& first_version,
                const std::string& second_version,
                const std::string& name,
                const std::string& version)
        {
            namespace except = core::exception;

            if ((first_comp_op == "<" || first_comp_op == "<=")
                && (second_comp_op == "<" || second_comp_op == "<="))
            {
                if (first_version > second_version) { // Prioritize the larger version
                    throw except::error(
                            "`" + name + ": " + version + "` is invalid expression.\n"
                            "Did you mean " + first_comp_op + first_version + " ?");
                }
                else {
                    throw except::error(
                            "`" + name + ": " + version + "` is invalid expression.\n"
                            "Did you mean " + second_comp_op + second_version + " ?");
                }
            }
            else if ((first_comp_op == ">" || first_comp_op == ">=")
                     && (second_comp_op == ">" || second_comp_op == ">="))
            {
                if (first_version < second_version) { // Prioritize the smaller version
                    throw except::error(
                            "`" + name + ": " + version + "` is invalid expression.\n"
                            "Did you mean " + first_comp_op + first_version + " ?");
                }
                else {
                    throw except::error(
                            "`" + name + ": " + version + "` is invalid expression.\n"
                            "Did you mean " + second_comp_op + second_version + " ?");
                }
            }
        }

        // Check if it is bounded interval
        //  (If it is unbounded, throw error)
        // (1, 6) => open bounded interval => OK!
        // [1, 6] => closed bounded interval => OK!
        // [a, ∞) => closed unbounded interval => one_exp
        // (-∞, ∞) => closed unbounded interval => ERR!
        // e.g. <0.1.1 and >=0.3.2
        void is_bounded_interval(
                const std::string& first_comp_op,
                const std::string& second_comp_op,
                const std::string& first_version,
                const std::string& second_version,
                const std::string& name,
                const std::string& version)
        {
            namespace except = core::exception;

            if (first_version < second_version) {
                if ((first_comp_op == "<" || first_comp_op == "<=")
                    && (second_comp_op == ">" || second_comp_op == ">="))
                {
                    throw except::error(
                            "`" + name + ": " + version + "` is strange.\n"
                            "In this case of interval specification using `and`,\n"
                            " it is necessary to be a bounded interval.\n"
                            "Please specify as in the following example:\n"
                            "e.g. `" + second_comp_op + first_version + " and "
                            + first_comp_op + second_version + "`");
                }
            }
            else if (first_version > second_version) {
                if ((first_comp_op == ">" || first_comp_op == ">=")
                    && (second_comp_op == "<" || second_comp_op == "<="))
                {
                    throw except::error(
                            "`" + name + ": " + version + "` is strange.\n"
                            "In this case of interval specification using `and`,\n"
                            " it is necessary to be a bounded interval.\n"
                            "Please specify as in the following example:\n"
                            "e.g. `" + first_comp_op + second_version + " and "
                            + second_comp_op + first_version + "`");
                }
            }
        }

        // Does the package exist in the version range
        template <class Predicate>
        std::vector<std::string>
        is_exists_in_version_range(
                const std::string& name,
                const std::string& version,
                Predicate pred)
        {
            namespace except = core::exception;

            boost::property_tree::ptree pt;
            {
                std::stringstream ss;
                ss << io::network::get(POAC_PACKAGES_API + name + "/versions");
                boost::property_tree::json_parser::read_json(ss, pt);
            }
            const auto vers = as_vector<std::string>(pt);

            std::vector<std::string> res;
            copy_if(vers.begin(), vers.end(), back_inserter(res), pred);
            if (res.size() == 0) {
                throw except::error("`" + name + ": " + version + "` was not found.");
            }
            else {
                return res;
            }
        }


        // name is boost/config, no boost-config
        std::optional<std::string>
        decide_version(const std::string& name, const std::string& version) {
            namespace except = core::exception;

            // TODO: 0.2.0-beta 等に未対応
            // TODO: 1.66.0 と 1.66 を同じとして扱えてなさそう

            std::string version_exp(R"(((v?)?(?:(\d+)(\.|_))?(?:(\d+)(\.|_))?(\*|\d+)))");
            std::string comp_op("(>=|>|<=|<)");
            std::regex one_exp("(^"+comp_op+"?"+version_exp+"$)");
            // no (>0.1.1 and 0.3.3)
            std::regex two_exp("(^"+comp_op+version_exp+"( and )"+comp_op+version_exp+"$)");

            std::smatch match_one;
            std::smatch match_two;
            if (std::regex_match(version, match_one, one_exp)) {
                const auto comp_op_str = match_one[2].str();
                const auto version_str = match_one[3].str();

                // Check それが等値なのかどうか．
                if (comp_op_str.empty()) { // equal
                    // Check 存在するかどうか
                    const std::string api_url = POAC_PACKAGES_API + name + "/" + version + "/exists";
                    if (io::network::get(api_url) == "true") {
                        return version;
                    }
                }

                const auto vers = is_exists_in_version_range( // TODO: これは，semverでやることじゃない
                        name, version,
                        [&](const std::string& s) {
                            if (comp_op_str == ">") {
                                return s > version_str;
                            }
                            else if (comp_op_str == ">=") {
                                return s >= version_str;
                            }
                            else if (comp_op_str == "<") {
                                return s < version_str;
                            }
                            else if (comp_op_str == "<=") {
                                return s <= version_str;
                            }
                            return false;
                        });
                return vers[0];
            }
            else if (std::regex_match(version, match_two, two_exp)) {
                const auto first_comp_op = match_two[2].str();
                const auto second_comp_op = match_two[11].str();
                const auto first_version = match_two[3].str();
                const auto second_version = match_two[12].str();

                // Checks
                is_wasteful_comparison_operation(
                        first_comp_op,
                        second_comp_op,
                        first_version,
                        second_version,
                        name, version);
                is_bounded_interval(
                        first_comp_op,
                        second_comp_op,
                        first_version,
                        second_version,
                        name, version);
                const auto vers = is_exists_in_version_range(
                        name, version,
                        [&](const std::string& s) {
                            if (first_comp_op == ">") {
                                if (second_comp_op == "<") {
                                    return (s > first_version && s < second_version);
                                }
                                else if (second_comp_op == "<=") {
                                    return (s > first_version && s <= second_version);
                                }
                            }
                            else if (first_comp_op == ">=") {
                                if (second_comp_op == "<") {
                                    return (s >= first_version && s < second_version);
                                }
                                else if (second_comp_op == "<=") {
                                    return (s >= first_version && s <= second_version);
                                }
                            }
                            else if (first_comp_op == "<") {
                                if (second_comp_op == ">") {
                                    return (s < first_version && s > second_version);
                                }
                                else if (second_comp_op == ">=") {
                                    return (s < first_version && s >= second_version);
                                }
                            }
                            else if (first_comp_op == "<=") {
                                if (second_comp_op == ">") {
                                    return (s <= first_version && s > second_version);
                                }
                                else if (second_comp_op == ">=") {
                                    return (s <= first_version && s >= second_version);
                                }
                            }
                            return false;
                        });
                // TODO: この部分の比較に関して正確ではないため，semverで比較する
                return vers[0];
            }
            else {
                throw except::error(
                        "`" + name + ": " + version +
                        "` is invalid expression.\n"
                        "Comparison operators:\n"
                        "  >, >=, <, <=\n"
                        "Logical operator:\n"
                        "  and\n"
                        "The following example is the meaning for equals:\n"
                        "  poac: \"1.2.0\"");
            }
        }

        void resolve() {
        }
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_HPP
