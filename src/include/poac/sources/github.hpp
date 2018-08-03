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
        // TODO: libcurl can not read redirect destination.
//        "https://github.com/curl/curl/archive/curl-7_61_0.tar.gz"
//    "https://github.com/boostorg/optional/archive/boost-1.67.0.tar.gz"
//    "https://github.com/opencv/opencv/archive/3.4.2.tar.gz"
//        "https://github.com/" + name + "/archive/" + tag + ".tar.gz"
//        -> redirect to
        return "https://codeload.github.com/" + name + "/tar.gz/" + tag;
    }

    // https://thinca.hatenablog.com/entry/20111006/1317832338
    void to_object(std::string* json) {
        json->insert(0, "{\"VALID\":");
        json->push_back('}');
    }
    std::string exists_url(const std::string& name, const std::string& tag) {
        // https://api.github.com/repos/curl/curl/contents/?ref=curl-7_61_0
        return "https://api.github.com/repos/"+name+"/contents/?ref="+tag;
    }
    bool installable(const std::string& name, const std::string& tag) {
        namespace pt = boost::property_tree;

        std::string temp = io::network::get_github(exists_url(name, tag));

        to_object(&temp);
        std::stringstream ss;
        ss << temp;

        pt::ptree json;
        pt::json_parser::read_json(ss, json);

        if (!json.get_optional<std::string>("VALID.message")) {
            for (const pt::ptree::value_type& child : json.get_child("VALID")) {
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
