#ifndef POAC_SOURCES_POAC_HPP
#define POAC_SOURCES_POAC_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../core/exception.hpp"
#include "../io/network.hpp"


namespace poac::sources::poac {
    std::string pkgname(const std::string& name, const std::string& version) {
        return name + "-" + version;
    }
    std::string resolve(const std::string& name, const std::string& version) {
        return "https://storage.googleapis.com/re.poac.pm/" + pkgname(name, version) + ".tar.gz";
    }

    bool is_equal_op(const std::string& version) {
        return !(version[0] == '>' || version[0] == '<');
    }

    template <typename T>
    std::vector<T> as_vector(const boost::property_tree::ptree& pt, const boost::property_tree::ptree::key_type& key)
    {
        std::vector<T> r;
        for (auto& item : pt.get_child(key))
            r.push_back(item.second.get_value<T>());
        return r;
    }

    boost::optional<std::string> decide_version(const std::string& name, const std::string& version) {
        namespace except = core::exception;

        // TODO: 0.2.0-beta 等に未対応
        // TODO: 1.66.0 と 1.66 を同じとして扱えてなさそう

        std::string version_exp("((v?)?(?:(\\d+)(\\.|_))?(?:(\\d+)(\\.|_))?(\\*|\\d+))");
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
                if (io::network::get("https://poac.pm/api/packages/" + name + "/" + version + "/exists") == "true") {
                    return version;
                }
            }

            // Check そのバージョン範囲でそのパッケージが存在するのか
            std::stringstream ss;
            ss << ("{\"dummy\": " + io::network::get("https://poac.pm/api/packages/" + name + "/versions") + "}");
            boost::property_tree::ptree pt;
            boost::property_tree::json_parser::read_json(ss, pt);
            const auto versions = as_vector<std::string>(pt, "dummy");
            const auto result = std::find_if(versions.begin(), versions.end(), [&](std::string s) {
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
            if (result == versions.end()) {
                throw except::error("`" + name + ": " + version + "` was not found.");
            }
            else { // Found
                return *result;
            }
        }
        else if (std::regex_match(version, match_two, two_exp)) {
            const std::string first_comp_op = match_two[2].str();
            const std::string second_comp_op = match_two[11].str();
            const std::string first_version = match_two[3].str();
            const std::string second_version = match_two[12].str();

            // Check 無駄な比較演算
            // >0.1.3 and >=0.3.2, <0.1.3 and <0.3.2
            if ((first_comp_op == "<" || first_comp_op == "<=") && (second_comp_op == "<" || second_comp_op == "<=")) {
                if (first_version > second_version) { // 大きい方を優先する．
                    throw except::error("`" + name + ": " + version + "` is invalid expression.\n"
                                        "Did you mean " + first_comp_op + first_version + " ?");
                }
                else {
                    throw except::error("`" + name + ": " + version + "` is invalid expression.\n"
                                        "Did you mean " + second_comp_op + second_version + " ?");
                }
            }
            else if ((first_comp_op == ">" || first_comp_op == ">=") && (second_comp_op == ">" || second_comp_op == ">=")) {
                if (first_version < second_version) { // 小さい方を優先する．
                    throw except::error("`" + name + ": " + version + "` is invalid expression.\n"
                                                        "Did you mean " + first_comp_op + first_version + " ?");
                }
                else {
                    throw except::error("`" + name + ": " + version + "` is invalid expression.\n"
                                                        "Did you mean " + second_comp_op + second_version + " ?");
                }
            }

            // Check 有界区間か (unboundedだとerror)
            // (1,6) => open bounded interval => OK!
            // [1, 6] => closed bounded interval => OK!
            // [a, ∞) => closed unbounded interval => one_exp
            // (-∞, ∞) => closed unbounded interval => ERR!
            // Like, <0.1.1 and >=0.3.2
            if (first_version < second_version) {
                if ((first_comp_op == "<" || first_comp_op == "<=") && (second_comp_op == ">" || second_comp_op == ">=")) {
                    throw except::error("`" + name + ": " + version + "` is strange.\n"
                                        "In the case of interval specification using `and`,\n"
                                        " it is necessary to be a bounded interval.\n"
                                        "Please specify as in the following example:\n"
                                        "e.g. `" + second_comp_op + first_version + " and " + first_comp_op + second_version + "`");
                }
            }
            else if (first_version > second_version) {
                if ((first_comp_op == ">" || first_comp_op == ">=") && (second_comp_op == "<" || second_comp_op == "<=")) {
                    throw except::error("`" + name + ": " + version + "` is strange.\n"
                                        "In the case of interval specification using `and`,\n"
                                        " it is necessary to be a bounded interval.\n"
                                        "Please specify as in the following example:\n"
                                        "e.g. `" + first_comp_op + second_version + " and " + second_comp_op + first_version + "`");
                }
            }

            // Check そのバージョン範囲でそのパッケージが存在するのか
            std::stringstream ss;
            ss << ("{\"dummy\": " + io::network::get("https://poac.pm/api/packages/" + name + "/versions") + "}");
            boost::property_tree::ptree pt;
            boost::property_tree::json_parser::read_json(ss, pt);
            const auto versions = as_vector<std::string>(pt, "dummy");
            // TODO: 複数バージョンが対象になる時、ちゃんとその中から最新のものが選ばれるのか分からない．
            const auto result = std::find_if(versions.begin(), versions.end(), [&](std::string s) {
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
            if (result == versions.end()) {
                std::cout << "joge???" << std::endl;
                throw except::error("`" + name + ": " + version + "` was not found.");
            }
            else { // Found
                return *result;
            }
        }
        else {
            throw except::error("`" + name + ": " + version + "` is invalid expression.\n"
                                "Comparison operators:\n"
                                "  >, >=, <, <=\n"
                                "Logical operator:\n"
                                "  and");
        }
    }

    // true == can install
    bool installable(const std::string& name, const std::string& version) {
        if (is_equal_op(version)) {
            return io::network::get("https://poac.pm/api/packages/" + name + "/" + version + "/exists") == "true";
        }
        else {
            return true;
        }
    }
    // resolve(name, version, architecture) in 0.6.0
} // end namespace
#endif // !POAC_SOURCES_POAC_HPP
