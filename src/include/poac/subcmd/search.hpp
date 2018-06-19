#ifndef POAC_SUBCMD_SEARCH_HPP
#define POAC_SUBCMD_SEARCH_HPP

#include <iostream>
#include <string>
#include <sstream>
//#include <optional>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../console.hpp"
#include "../utility/requests.hpp"


namespace poac::subcmd { struct search {
    static const std::string summary() { return "Search for packages in poacpm."; }
    static const std::string options() { return "<pkg-name>"; }

    template <typename VS>
    void operator()(VS&& vs) { _main(vs); }
    template <typename VS>
    void _main(VS&& vs) {
        using namespace boost::property_tree;

        const std::string url("https://poac.pm/api/v1/packages?search=" + vs[0]);
        std::stringstream ss;
        ss << poac::utility::requests::get(url);
        ptree pt;
        json_parser::read_json(ss, pt);

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
                std::cout << "package is nothing" << std::endl;
            }
        }
        if (now_count == 0) echo_not_founded();
    }
    void echo_first_line() {
        std::cout << poac::console::underline
                  << "Package"
                  << poac::console::reset
                  << std::endl;
    }
    void echo_not_founded() {
        std::cerr << poac::console::red
                  << "package not founded"
                  << poac::console::reset
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }
};} // end namespace
#endif // !POAC_SUBCMD_SEARCH_HPP
