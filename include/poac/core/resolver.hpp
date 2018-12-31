#ifndef POAC_CORE_RESOLVER_HPP
#define POAC_CORE_RESOLVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <utility>
#include <optional>
#include <algorithm> // copy_if
#include <iterator> // back_inserter

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <yaml-cpp/yaml.h>
#include <cvc4/cvc4.h>

#include "exception.hpp"
#include "naming.hpp"
#include "semver.hpp"
#include "../io/file.hpp"
#include "../io/network.hpp"


namespace poac::core::resolver {
    namespace cache {
        bool resolve(const std::string& package_name) {
            namespace path = io::file::path;
            const auto package_path = path::poac_cache_dir / package_name;
            return path::validate_dir(package_path);
        }
    }

    namespace current {
        bool resolve(const std::string& current_package_name) {
            namespace path = io::file::path;
            const auto package_path = path::current_deps_dir / current_package_name;
            return path::validate_dir(package_path);
        }
    }

    namespace github {
        std::string archive_url(const std::string& name, const std::string& tag) {
            return "https://github.com/" + name + "/archive/" + tag + ".tar.gz";
        }
        std::string archive_url(const std::string& name) {
            return "https://github.com/" + name + ".git";
        }
    }

    namespace poac {
        std::string archive_url(const std::string& name, const std::string& version) {
            return "https://storage.googleapis.com/poac-pm.appspot.com/"
                   + core::naming::to_cache("poac", name, version) + ".tar.gz";
        }

        // boost::property_tree::ptree : {"key": ["array", "...", ...]}
        //  -> std::vector<T> : ["array", "...", ...]
        template <typename T, typename U, typename K=typename U::key_type>
        std::vector<T> as_vector(const U& pt, const K& key) { // as_vector(pt, "key")
            std::vector<T> r;
            for (const auto& item : pt.get_child(key)) {
                r.push_back(item.second.template get_value<T>());
            }
            return r;
        }
        // boost::property_tree::ptree : ["array", "...", ...]
        //  -> std::vector<T> : ["array", "...", ...]
        template <typename T, typename U>
        std::vector<T> as_vector(const U& pt) {
            std::vector<T> r;
            for (const auto& item : pt) {
                r.push_back(item.second.template get_value<T>());
            }
            return r;
        }

        // name is boost/config, no boost-config
        std::optional<std::string>
        decide_version(const std::string& name, const std::string& version_range) {
            // TODO: (`>1.2 and <=1.3.2` -> NG，`>1.2.0-alpha and <=1.3.2` -> OK)
            boost::property_tree::ptree pt;
            {
                std::stringstream ss;
                ss << io::network::get(POAC_PACKAGES_API + name + "/versions");
                boost::property_tree::json_parser::read_json(ss, pt);
            }
            const auto versions = as_vector<std::string>(pt);

            std::vector<std::string> res;
            semver::Interval interval(name, version_range);
            copy_if(versions.begin(), versions.end(), back_inserter(res),
                    [&](std::string s){ return interval.satisfies(s); });
            if (res.empty()) {
                throw exception::error("`" + name + ": " + version_range + "` was not found.");
            }
            else {
                return res[0];
            }
        }

        void resolve() {
        }
    }
} // end namespace
#endif // !POAC_CORE_RESOLVER_HPP
