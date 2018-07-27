#ifndef POAC_SUBCMD_SEARCH_HPP
#define POAC_SUBCMD_SEARCH_HPP

#include <iostream>
#include <string>
#include <sstream>
//#include <optional>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../core/except.hpp"
#include "../io/cli.hpp"
#include "../io/network.hpp"


namespace poac::subcmd { struct search {
    static const std::string summary() { return "Beta: Search for packages in poacpm."; }
    static const std::string options() { return "<pkg-name>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main(VS&& vs) {
        using namespace boost::property_tree;

        if (vs.size() != 1) throw poac::core::invalid_second_argument("search");
        const std::string url("https://poac.pm/api/v1/packages?search=" + vs[0]);
        std::stringstream ss;
        ss << poac::io::network::get(url);
        ptree pt;
        json_parser::read_json(ss, pt);

        // TODO: Check if connecting network

        unsigned int now_count = 0;
        const unsigned int max_count = 20;
        for (const ptree::value_type& child : pt.get_child("packages")) {
            const ptree& info = child.second;
            // packages.name
            if (boost::optional<std::string> name = info.get_optional<std::string>("name")) {
                if (now_count == 0) echo_first_line();
                std::cout << name.get() << std::endl;
                if (++now_count >= max_count) break;
            }
            else {
                std::cout << "name is nothing" << std::endl;
            }
        }
        if (now_count == 0) echo_not_founded(vs[0]);
    }
    void echo_first_line() {
        std::cout << poac::io::cli::underline << "User/Package" << poac::io::cli::reset << "        "
                  << poac::io::cli::underline << "Description" << poac::io::cli::reset << "                             "
                  << poac::io::cli::underline << "Version" << poac::io::cli::reset << "        "
                  << poac::io::cli::underline << "Tags" << poac::io::cli::reset
                  << std::endl;
    }
    void echo_not_founded(const std::string& s) {
        std::cerr << poac::io::cli::red
                  << s << " not found"
                  << poac::io::cli::reset
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
};} // end namespace
#endif // !POAC_SUBCMD_SEARCH_HPP
