#ifndef STROITE_UTILS_OPTIONS_HPP
#define STROITE_UTILS_OPTIONS_HPP

#include <string>
#include <vector>

#include <boost/filesystem.hpp>


namespace stroite::utils::options {
    struct compile {
        std::string system;
        std::string version_prefix;
        unsigned int cpp_version;
        std::vector<std::string> source_files;
        std::vector<std::string> include_search_path;
        std::vector<std::string> other_args;
        std::vector<std::string> macro_defns;
        boost::filesystem::path base_dir;
        boost::filesystem::path output_root;
        bool verbose;
    };

    struct link {
        std::string system;
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
        std::vector<std::string> library_search_path;
        std::vector<std::string> static_link_libs;
        std::vector<std::string> library_path;
        std::vector<std::string> other_args;
        bool verbose;
    };

    struct static_lib {
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
        bool verbose;
    };

    struct dynamic_lib {
        std::string system;
        std::string project_name;
        boost::filesystem::path output_root;
        std::vector<std::string> obj_files_path;
        bool verbose;
    };
} // end namespace
#endif // STROITE_UTILS_OPTIONS_HPP
