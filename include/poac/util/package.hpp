#ifndef POAC_UTIL_PACKAGE_HPP
#define POAC_UTIL_PACKAGE_HPP

#include <string>
#include <regex>

#include <yaml-cpp/yaml.h>

#include "../core/exception.hpp"
#include "../io/file/yaml.hpp"


namespace poac::util::package {
    // username/repository -> repository
    std::string basename(const std::string& name) {
        if (name.find('/') != std::string::npos)
            return std::string(name, name.find('/')+1);
        else
            return name;
    }


    // 1. opencv/opencv, 3.4.2 -> opencv-3.4.2
    // 2. curl/curl, 7_61_0 -> curl-7.61.0
    // 3. matken11235/logger_slack_backend, v0.1.0 -> logger_slack_backend-0.1.0
    // 4. curl/curl, curl-7_61_0 -> curl-7_61_0
    // 5. boostorg/optional, boost-1.67.0 -> boost-optional-1.67.0

    // GitHub
    std::string github_conv_pkgname(const std::string& name, const std::string& tag) {
        std::regex pattern("^(v?)?(?:(\\d+)(\\.|_))?(?:(\\d+)(\\.|_))?(\\*|\\d+)$");
        std::smatch matches;

        // 1. boost -> boost
        // 2. matken11235/logger_slack_backend -> logger_slack_backend
        // ...
        std::string prefix = basename(name);

        // 1, 2, 3
        if (std::regex_match(tag, matches, pattern)) {
            // 1. Group1: ``,  Group2: `3`, Group3: `.`, Group4: `4`,  Group5: `.`, Group6: `2`
            // 2. Group1: ``,  Group2: `7`, Group3: `_`, Group4: `61`, Group5: `_`, Group6: `0`
            // 3. Group1: `v`, Group2: `0`, Group3: `.`, Group4: `1`,  Group5: `.`, Group6: `0`
            return prefix + "-" + matches[2].str() + "." + matches[4].str() + "." + matches[6].str();
        }
        // 4, 5
        else {
            // TODO: not yet 5
            return tag;
        }
    }

    // opencv-3.4.2 -> opencv
    // cache to current
    std::string cache_to_current(const std::string& pkgname) {
        if (const auto pos = pkgname.find_last_of('-'); pos != std::string::npos) {
            return std::string(pkgname, 0, pos);
        }
        else {
            return pkgname;
        }
    }


    // 1. boost, 1.67.0 -> boost-1.67.0
    // 2. boost::optional, 1.67.0 -> boost::optional-1.67.0

    // poac
    std::string poac_conv_pkgname(const std::string& name, const std::string& tag) {
        return name + "-" + tag;
    }


    std::string get_version(const YAML::Node& node, const std::string& source) {
        namespace except = core::exception;

        if (source == "github")
            return node["tag"].as<std::string>(); // TODO: 書かれていなかった時，main.cppまでexceptが飛んでしまう．
        else if (source == "poac")
            return node["version"].as<std::string>();
        else
            throw except::error("poac.yml error\nWhat source is " + source + "?");
    }

    std::string get_source(const YAML::Node& node) {
        if (const auto src = io::file::yaml::get_by_width(node, "src"))
            return (*src).at("src").as<std::string>();
        else
            return "poac";
    }
} // end namespace
#endif // !POAC_UTIL_PACKAGE_HPP
