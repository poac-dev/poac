#ifndef POAC_SUBCMD_SEARCH_HPP
#define POAC_SUBCMD_SEARCH_HPP

#include <iostream>
#include <string>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../core/exception.hpp"
#include "../io/cli.hpp"
#include "../io/network.hpp"
#include "../util/argparse.hpp"


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
            namespace except = core::exception;

            std::string params;
            {
                boost::property_tree::ptree pt;
                pt.put("params", "query=" + query + "&hitsPerPage=20");
                std::stringstream ss;
                boost::property_tree::json_parser::write_json(ss, pt);
                params = ss.str();
            }

            std::stringstream ss;
            io::network::Headers headers;
            headers.emplace("X-Algolia-API-Key", ALGOLIA_SEARCH_ONLY_KEY);
            headers.emplace("X-Algolia-Application-Id", ALGOLIA_APPLICATION_ID);
            ss << io::network::post(ALGOLIA_SEARCH_INDEX_API, params, ALGOLIA_SEARCH_INDEX_API_HOST, headers);

            boost::property_tree::ptree pt;
            boost::property_tree::json_parser::read_json(ss, pt);
            if (const auto value = pt.get_optional<int>("nbHits")) {
                if (*value <= 0) {
                    throw except::error(query + " not found");
                }
            }
            return pt;
        }

        // If string size is over specified number of characters and it can be clipped,
        //  display an ellipsis (...).
        std::string string_pretty(const std::string& s, const unsigned long& n) {
            if (s.size() <= n) {
                return s;
            }
            else {
                return s.substr(0, n) + "...";
            }
        }

        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void _main(VS&& argv) {
            using namespace boost::property_tree;
            namespace except = core::exception;

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
                io::cli::set_left(25);
                std::cout << string_pretty(hits.get<std::string>("name"), 21);
                io::cli::set_left(50);
                std::cout << string_pretty(hits.get<std::string>("description"), 45);
                io::cli::set_left(15);
                std::cout << hits.get<std::string>("version")
                          << "    " << hits.get<std::string>("cpp_version")
                          << std::endl;
            }
        }

        void check_arguments(const std::vector<std::string>& argv) {
            namespace except = core::exception;
            if (argv.size() != 1) {
                throw except::invalid_second_arg("search");
            }
        }
    }

    struct search {
        static const std::string summary() {
            return "Search for packages in poac.pm";
        }
        static const std::string options() {
            return "<pkg-name>";
        }
        template<typename VS, typename=std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
        void operator()(VS&& argv) {
            _search::check_arguments(argv);
            _search::_main(std::move(argv));
        }
    };
} // end namespace
#endif // !POAC_SUBCMD_SEARCH_HPP
