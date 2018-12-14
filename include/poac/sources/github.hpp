#ifndef POAC_SOURCES_GITHUB_HPP
#define POAC_SOURCES_GITHUB_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../io/network.hpp"


namespace poac::sources::github {
    std::string resolve(const std::string& name, const std::string& tag) {
        return "https://github.com/" + name + "/archive/" + tag + ".tar.gz";
    }
    std::string resolve(const std::string& name) {
        return "https://github.com/" + name + ".git";
    }
} // end namespace
#endif // !POAC_SOURCES_GITHUB_HPP
