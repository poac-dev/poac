#ifndef POAC_SOURCES_POAC_HPP
#define POAC_SOURCES_POAC_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "../io/network.hpp"


namespace poac::sources::poac {
    std::string pkgname(const std::string& name, const std::string& version) {
        return name + "-" + version;
    }
    std::string resolve(const std::string& name, const std::string& version) {
        return "https://storage.googleapis.com/re.poac.pm/" + pkgname(name, version) + ".tar.gz";
    }
    // true == can install
    bool installable(const std::string& name, const std::string& version) {
        const std::string query = "?name=" + name + "&version=" + version;
        return io::network::get("https://poac.pm/api/v1/packages/exists" + query) == "true";
    }
    // resolve(name, version, architecture) in 0.6.0
} // end namespace
#endif // !POAC_SOURCES_POAC_HPP
