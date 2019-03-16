#ifndef POAC_SUBCMD_SEARCH_HPP
#define POAC_SUBCMD_SEARCH_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../core/exception.hpp"
#include "../io/cli.hpp"
#include "../io/net.hpp"
#include "../util/argparse.hpp"
#include "../util/pretty.hpp"


namespace poac::subcmd {
    namespace _search {
        void echo_first_line() {
            const int ul_size = static_cast<int>(io::cli::underline.size());
            const int reset_size = static_cast<int>(io::cli::reset.size());

            io::cli::set_left(ul_size + 25 + reset_size);
            std::cout << io::cli::to_underline("Package");
            io::cli::set_left(ul_size + 50 + reset_size);
            std::cout << io::cli::to_underline("Description");
            io::cli::set_left(ul_size + 15 + reset_size);
            std::cout << io::cli::to_underline("Version")
                      << io::cli::to_underline("C++ Version")
                      << std::endl;
        }

        boost::property_tree::ptree get_search_api(const std::string& query) {
            namespace exception = core::exception;

            std::string params;
            {
                boost::property_tree::ptree pt;
                pt.put("params", "query=" + query + "&hitsPerPage=20");
                std::stringstream ss;
                boost::property_tree::json_parser::write_json(ss, pt);
                params = ss.str();
            }

            std::stringstream ss;
            io::net::Headers headers;
            headers.emplace("X-Algolia-API-Key", ALGOLIA_SEARCH_ONLY_KEY);
            headers.emplace("X-Algolia-Application-Id", ALGOLIA_APPLICATION_ID);
            const io::net::requests req{ ALGOLIA_SEARCH_INDEX_API_HOST };
            const auto res = req.post(ALGOLIA_SEARCH_INDEX_API, params, headers);
            ss << res.data();

            boost::property_tree::ptree pt;
            boost::property_tree::json_parser::read_json(ss, pt);
            if (const auto value = pt.get_optional<int>("nbHits")) {
                if (*value <= 0) {
                    throw exception::error(exception::msg::not_found(query));
                }
            }
            return pt;
        }

        // https://marycore.jp/prog/cpp/std-string-replace-first-all/#std%3A%3Areplace関数による全置換
        unsigned int replace(std::string& s, const std::string& from, const std::string& target) {
            std::size_t from_length = from.size();
            std::size_t target_length = target.size();
            std::size_t pos = 0;
            unsigned int count = 0;
            while ((pos = s.find(from, pos)) != std::string::npos) {
                s.replace(pos, from_length, target);
                pos += target_length;
                ++count;
            }
            return count;
        }

        template<typename VS>
        int _main(VS&& argv) {
            namespace cli = io::cli;
            using namespace boost::property_tree;

            const bool verbose = util::argparse::use(argv, "-v", "--verbose");

            const auto pt = get_search_api(argv[0]);
            if (verbose) {
                std::stringstream ss;
                json_parser::write_json(ss, pt);
                std::cout << ss.str() << std::endl;
            }

            echo_first_line();
            for (const ptree::value_type& child : pt.get_child("hits")) {
                const ptree& hits = child.second;

                std::string name = hits.get<std::string>("_highlightResult.name.value");
                auto count_s = replace(name, "<em>", cli::red) * cli::red.size();
                auto count_l = replace(name, "</em>", cli::reset) * cli::reset.size();

                cli::set_left(25 + count_s + count_l);
                std::cout << util::pretty::clip_string(name, 21 + count_s + count_l);
                cli::set_left(50);
                std::cout << util::pretty::clip_string(hits.get<std::string>("description"), 45);
                cli::set_left(15);
                const auto cpp_version = hits.get<std::string>("cpp_version");
                std::cout << hits.get<std::string>("version")
                          << "    " << (cpp_version == "3" ? "03" : cpp_version)
                          << std::endl;
            }

            return EXIT_SUCCESS;
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace exception = core::exception;
            if (argv.size() != 1) {
                throw exception::invalid_second_arg("search");
            }
        }
    }

    struct search {
        static std::string summary() {
            return "Search for packages in poac.pm";
        }
        static std::string options() {
            return "<pkg-name>";
        }
        template<typename VS>
        int operator()(VS&& argv) {
            _search::check_arguments(argv);
            return _search::_main(std::forward<VS>(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_SEARCH_HPP
