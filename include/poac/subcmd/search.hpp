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


namespace poac::subcmd { struct search {
    static const std::string summary() { return "Beta: Search for packages in poacpm."; }
    static const std::string options() { return "<pkg-name>"; }

//    const std::string url = "https://poac-test.herokuapp.com";
    const std::string url = "https://poac.pm/api/v1";

    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void operator()(VS&& argv) { _main(std::move(argv)); }
    template <typename VS, typename = std::enable_if_t<std::is_rvalue_reference_v<VS&&>>>
    void _main(VS&& argv) {
        using namespace boost::property_tree;
        namespace except = core::exception;

        check_arguments(argv);
        const bool verbose = util::argparse::use(argv, "-v", "--verbose");

        const auto pt = get_search_api(argv[0]);
        if (verbose) {
            std::stringstream ss;
            json_parser::write_json(ss, pt);
            std::cout << ss.str() << std::endl;
        }

        echo_first_line();
        for (const ptree::value_type& child : pt.get_child("packages")) {
            const ptree& info = child.second;
            io::cli::set_left(15);
            std::cout << info.get<std::string>("name");
            io::cli::set_left(50);
            std::cout << info.get<std::string>("description");
            io::cli::set_left(15);
            std::cout << info.get<std::string>("version")
                      << "    " << info.get<std::string>("cpp_version")
                      << std::endl;
        }
    }

    void echo_first_line() {
        const int ulsize = io::cli::underline.size();
        const int rsetsize = io::cli::reset.size();

        io::cli::set_left(ulsize + 15 + rsetsize);
        std::cout << io::cli::to_underline("Package");
        io::cli::set_left(ulsize + 50 + rsetsize);
        std::cout << io::cli::to_underline("Description");
        io::cli::set_left(ulsize + 15 + rsetsize);
        std::cout << io::cli::to_underline("Version")
                  << io::cli::to_underline("C++ Version")
                  << std::endl;
    }

    boost::property_tree::ptree get_search_api(const std::string& query) {
        namespace except = core::exception;

        std::stringstream ss;
        ss << io::network::get(url + "/search?q=" + query);
        if (ss.str() == "notfound")
            throw except::error(query + " not found");

        boost::property_tree::ptree pt;
        boost::property_tree::json_parser::read_json(ss, pt);
        return pt;
    }

    void check_arguments(const std::vector<std::string>& argv) {
        namespace except = core::exception;

        if (argv.size() < 1)
            throw except::invalid_second_arg("search");
    }
};} // end namespace
#endif // !POAC_SUBCMD_SEARCH_HPP
