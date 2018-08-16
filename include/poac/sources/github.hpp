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


/*
 * Installability:
 *   1. Exist poac.(yml|yaml) in project root. (Resolve dependencies with poac)
 *   2. Exist CMakeLists.txt in project root.  (Build required)
 *   3. Exist include directory. (header-only)
 */
namespace poac::sources::github {
    std::string resolve(const std::string& name, const std::string& tag) {
        return "https://github.com/" + name + "/archive/" + tag + ".tar.gz";
    }
    std::string resolve(const std::string& name) {
        return "https://github.com/" + name + ".git";
    }

    std::string exists_url(const std::string& name, const std::string& tag) {
        // https://api.github.com/repos/curl/curl/contents/?ref=curl-7_61_0
        return "https://api.github.com/repos/"+name+"/contents/?ref="+tag;
    }
    bool installable(const std::string& name, const std::string& tag) {
        namespace pt = boost::property_tree;

        std::stringstream ss;
        ss << io::network::get_github(exists_url(name, tag));

        pt::ptree json;
        pt::json_parser::read_json(ss, json);

        if (!json.get_optional<std::string>("message")) {
            for (const pt::ptree::value_type& child : json.get_child("")) {
                const pt::ptree& info = child.second;
                if (boost::optional<std::string> name = info.get_optional<std::string>("name")) {
                    if (boost::optional<std::string> type = info.get_optional<std::string>("type")) {
                        if (name.get() == "poac.yml" && type.get() == "file")
                            return true;
                        else if (name.get() == "poac.yaml" && type.get() == "file")
                            return true;
                        else if (name.get() == "CMakeLists.txt" && type.get() == "file")
                            return true;
                        else if (name.get() == "include" && type.get() == "dir")
                            return true;
                    }
                }
            }
        }
        return false;
    }
} // end namespace
#endif // !POAC_SOURCES_GITHUB_HPP
