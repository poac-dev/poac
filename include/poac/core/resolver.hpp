#ifndef POAC_CORE_RESOLVER_HPP
#define POAC_CORE_RESOLVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <utility>
#include <optional>

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

        template <typename T, typename U, typename K=typename U::key_type>
        std::vector<T> as_vector(const U& pt, const K& key) {
            std::vector<T> r;
            for (const auto& item : pt.get_child(key)) {
                r.push_back(item.second.template get_value<T>());
            }
            return r;
        }

        // e.g. >0.1.3 and >=0.3.2, <0.1.3 and <0.3.2
        void is_useless_comparison_operation(
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

        // name is boost/config, no boost-config
        std::optional<std::string>
        decide_version(const std::string& name, const std::string& version) {
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
                    const std::string api_url = POAC_API_PACKAGES + name + "/" + version + "/exists";
                    if (io::network::get(api_url) == "true") {
                        return version;
                    }
                }

                // Check そのバージョン範囲でそのパッケージが存在するのか
                std::stringstream ss;
                ss << ("{\"dummy\": " + io::network::get(POAC_API_PACKAGES + name + "/versions") + "}");
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
                const auto first_comp_op = match_two[2].str();
                const auto second_comp_op = match_two[11].str();
                const auto first_version = match_two[3].str();
                const auto second_version = match_two[12].str();

                is_useless_comparison_operation(
                        first_comp_op,
                        second_comp_op,
                        first_version,
                        second_version,
                        name, version);

                // is_bounded_interval
                // Check 有界区間か (unboundedだとerror)
                // (1,6) => open bounded interval => OK!
                // [1, 6] => closed bounded interval => OK!
                // [a, ∞) => closed unbounded interval => one_exp
                // (-∞, ∞) => closed unbounded interval => ERR!
                // Like, <0.1.1 and >=0.3.2
                if (first_version < second_version) {
                    if ((first_comp_op == "<" || first_comp_op == "<=")
                        && (second_comp_op == ">" || second_comp_op == ">="))
                    {
                        throw except::error(
                                "`" + name + ": " + version + "` is strange.\n"
                                "In the case of interval specification using `and`,\n"
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
                                "In the case of interval specification using `and`,\n"
                                " it is necessary to be a bounded interval.\n"
                                "Please specify as in the following example:\n"
                                "e.g. `" + first_comp_op + second_version + " and "
                                + second_comp_op + first_version + "`");
                    }
                }

                // Check そのバージョン範囲でそのパッケージが存在するのか
                std::stringstream ss;
                ss << ("{\"dummy\": " + io::network::get(POAC_API_PACKAGES + name + "/versions") + "}");
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
                    throw except::error("`" + name + ": " + version + "` was not found.");
                }
                else { // Found
                    return *result;
                }
            }
            else {
                throw except::error("`" + name + ": " + version
                                    + "` is invalid expression.\n"
                                    "Comparison operators:\n"
                                    "  >, >=, <, <=\n"
                                    "Logical operator:\n"
                                    "  and");
            }
        }

        void resolve() {
            CVC4::ExprManager em;
            CVC4::SmtEngine smt(&em);

//        smt.setOption("incremental", CVC4::SExpr("true")); // Enable incremental solving
//        CVC4::Type real = em.realType();
//        CVC4::Type integer = em.integerType();
//        CVC4::Expr x = em.mkVar("x", integer);
//        CVC4::Expr y = em.mkVar("y", real);
//
//        CVC4::Expr three = em.mkConst(CVC4::Rational(3));
//        CVC4::Expr neg2 = em.mkConst(CVC4::Rational(-2));
//        CVC4::Expr two_thirds = em.mkConst(CVC4::Rational(2, 3));
//
//        CVC4::Expr three_y = em.mkExpr(CVC4::kind::MULT, three, y);
//        CVC4::Expr diff = em.mkExpr(CVC4::kind::MINUS, y, x);
//
//        CVC4::Expr x_geq_3y = em.mkExpr(CVC4::kind::GEQ, x, three_y);
//        CVC4::Expr x_leq_y = em.mkExpr(CVC4::kind::LEQ, x, y);
//        CVC4::Expr neg2_lt_x = em.mkExpr(CVC4::kind::LT, neg2, x);
//
//        CVC4::Expr assumptions =
//                em.mkExpr(CVC4::kind::AND, x_geq_3y, x_leq_y, neg2_lt_x);
//        smt.assertFormula(assumptions);
//
//        smt.push();
//        CVC4::Expr diff_leq_two_thirds = em.mkExpr(CVC4::kind::LEQ, diff, two_thirds);
//        std::cout << "Prove that " << diff_leq_two_thirds << " with CVC4." << std::endl;
//        std::cout << "CVC4 should report VALID." << std::endl;
//        std::cout << "Result from CVC4 is: " << smt.query(diff_leq_two_thirds) << std::endl;
//        smt.pop();
//
//        std::cout << std::endl;
//
//        smt.push();
//        CVC4::Expr diff_is_two_thirds = em.mkExpr(CVC4::kind::EQUAL, diff, two_thirds);
//        smt.assertFormula(diff_is_two_thirds);
//        std::cout << "Show that the asserts are consistent with " << std::endl;
//        std::cout << diff_is_two_thirds << " with CVC4." << std::endl;
//        std::cout << "CVC4 should report SAT." << std::endl;
//        std::cout << "Result from CVC4 is: " << smt.checkSat(em.mkConst(true)) << std::endl;
//        smt.pop();

            smt.setLogic("S");
            CVC4::Type string = em.stringType();
            // std::string
            std::string std_str_ab("ab");
            // CVC4::String
            CVC4::String cvc4_str_ab(std_str_ab);
            CVC4::String cvc4_str_abc("abc");
            // String constants
            CVC4::Expr ab = em.mkConst(cvc4_str_ab);
            CVC4::Expr abc = em.mkConst(CVC4::String("abc"));

            CVC4::Expr x = em.mkVar("x", string);
            CVC4::Expr y = em.mkVar("y", string);
            CVC4::Expr z = em.mkVar("z", string);

            CVC4::Expr lhs = em.mkExpr(CVC4::kind::STRING_CONCAT, x, ab, y);
            // String concatenation: abc.z
            CVC4::Expr rhs = em.mkExpr(CVC4::kind::STRING_CONCAT, abc, z);
            // x.ab.y = abc.z
            CVC4::Expr formula1 = em.mkExpr(CVC4::kind::EQUAL, lhs, rhs);
            // Length of y: |y|
            CVC4::Expr leny = em.mkExpr(CVC4::kind::STRING_LENGTH, y);
            // |y| >= 0
            CVC4::Expr formula2 = em.mkExpr(CVC4::kind::GEQ, leny, em.mkConst(CVC4::Rational(0)));
            // Regular expression: (ab[c-e]*f)|g|h
            CVC4::Expr r = em.mkExpr(CVC4::kind::REGEXP_UNION,
                                     em.mkExpr(CVC4::kind::REGEXP_CONCAT,
                                               em.mkExpr(CVC4::kind::STRING_TO_REGEXP,
                                                       em.mkConst(CVC4::String("ab"))),
                                               em.mkExpr(CVC4::kind::REGEXP_STAR,
                                                         em.mkExpr(CVC4::kind::REGEXP_RANGE,
                                                                   em.mkConst(CVC4::String("c")),
                                                                   em.mkConst(CVC4::String("e")))),
                                               em.mkExpr(CVC4::kind::STRING_TO_REGEXP, em.mkConst(CVC4::String("f")))),
                                     em.mkExpr(CVC4::kind::STRING_TO_REGEXP, em.mkConst(CVC4::String("g"))),
                                     em.mkExpr(CVC4::kind::STRING_TO_REGEXP, em.mkConst(CVC4::String("h"))));
            // String variables
            CVC4::Expr s1 = em.mkVar("s1", string);
            CVC4::Expr s2 = em.mkVar("s2", string);
            // String concatenation: s1.s2
            CVC4::Expr s = em.mkExpr(CVC4::kind::STRING_CONCAT, s1, s2);
            // s1.s2 in (ab[c-e]*f)|g|h
            CVC4::Expr formula3 = em.mkExpr(CVC4::kind::STRING_IN_REGEXP, s, r);

            CVC4::Expr q = em.mkExpr(CVC4::kind::AND,
                                     formula1,
                                     formula2,
                                     formula3);

            CVC4::Result result = smt.checkSat(q);
            std::cout << "CVC4 reports: " << q << " is " << result << "." << std::endl;
            if (result == CVC4::Result::SAT) {
                std::cout << " x = " << smt.getValue(x) << std::endl;
                std::cout << " s1.s2 = " << smt.getValue(s) << std::endl;
            }
        }
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_HPP
