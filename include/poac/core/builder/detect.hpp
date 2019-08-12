#ifndef POAC_CORE_BUILDER_DETECT_HPP
#define POAC_CORE_BUILDER_DETECT_HPP

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <poac/core/except.hpp>
#include <poac/io/config.hpp>
#include <poac/io/path.hpp>
#include <poac/util/shell.hpp>

namespace poac::core::builder::detect {
    std::string check_support_build_system(const std::string& system) {
        if (system != "poac" && system != "cmake") {
            throw except::error("Unknown build system ", system);
        }
        return system;
    }

//    std::optional<std::string>
//    build_system(const YAML::Node& node) {
//        if (const auto system = io::config::detail::get<std::string>(node, "build")) {
//            return check_support_build_system(*system);
//        }
//        else if (const auto build_node = io::config::detail::get<std::map<std::string, YAML::Node>>(node, "build")) {
//            YAML::Node build_node2;
//            try {
//                build_node2 = (*build_node).at("system");
//            } catch(std::out_of_range&) {
//                return std::nullopt;
//            }
//
//            if (const auto system2 = io::config::detail::get<std::string>(build_node2)) {
//                return check_support_build_system(*system2);
//            }
//        }
//        // No build required
//        return std::nullopt;
//    }

    bool is_cpp_file(const std::filesystem::path& p) {
        namespace fs = std::filesystem;
        return !fs::is_directory(p)
               && (p.extension().string() == ".cpp"
                   || p.extension().string() == ".cxx"
                   || p.extension().string() == ".cc"
                   || p.extension().string() == ".cp");
    }

    std::vector<std::string>
    search_cpp_file(const std::filesystem::path& base_dir) {
        namespace fs = std::filesystem;
        namespace path = io::path;

        std::vector<std::string> source_files;
        const auto source_dir = base_dir / "src";
        if (path::validate_dir(source_dir)) {
            for (const fs::path& p : fs::recursive_directory_iterator(source_dir)) {
                if (is_cpp_file(p)) {
                    source_files.push_back(p.string());
                }
            }
        }
        return source_files;
    }
} // end namespace
#endif // POAC_CORE_BUILDER_DETECT_HPP
