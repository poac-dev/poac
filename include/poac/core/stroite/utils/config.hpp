#ifndef POAC_CORE_STROITE_UTILS_CONFIG_HPP
#define POAC_CORE_STROITE_UTILS_CONFIG_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include "../../exception.hpp"
#include "../../../util/command.hpp"


namespace poac::core::stroite::utils::config {
    template <typename Opts>
    void enable_gnu(Opts& opts) { // TODO:
        opts.version_prefix = "-std=gnu++";
    }
    std::string default_version_prefix() {
        return "-std=c++";
    }
    template <typename T>
    std::string make_macro_defn(const std::string& first, const T& second) {
        return make_macro_defn(first, std::to_string(second));
    }
    template <>
    std::string make_macro_defn<std::string>(const std::string& first, const std::string& second) {
        return "-D" + first + "=" + R"(\")" + second + R"(\")";
    }
    template <>
    std::string make_macro_defn<std::uint64_t>(const std::string& first, const std::uint64_t& second) {
        std::ostringstream oss;
        oss << second;
        return make_macro_defn(first, oss.str());
    }

    // Automatic selection of compiler
    auto auto_select_compiler() {
        namespace command = util::_command;

        if (const char* cxx = std::getenv("CXX")) {
            return cxx;
        }
        else if (command::has_command("g++")) {
            return "g++";
        }
        else if (command::has_command("clang++")) {
            return "clang++";
        }
        else {
            throw exception::error(
                    "Environment variable \"CXX\" was not found.\n"
                    "Select the compiler and export it.");
        }
    }


    std::string check_support_build_system(const std::string& system) {
        if (system != "poac" && system != "cmake") {
            throw exception::error("Unknown build system " + system);
        }
        return system;
    }

    std::optional<std::string>
    detect_build_system(const YAML::Node& node)
    {
        namespace yaml = io::file::yaml;

        if (const auto system = yaml::get<std::string>(node, "build")) {
            return check_support_build_system(*system);
        }
        else if (const auto build_node = yaml::get<std::map<std::string, YAML::Node>>(node, "build")) {
            YAML::Node build_node2;
            try {
                build_node2 = (*build_node).at("system");
            }
            catch(std::out_of_range&) {
                return std::nullopt;
            }

            if (const auto system2 = yaml::get<std::string>(build_node2)) {
                return check_support_build_system(*system2);
            }
        }
        // No build required
        return std::nullopt;
    }
} // end namespace
#endif // POAC_CORE_STROITE_UTILS_CONFIG_HPP
