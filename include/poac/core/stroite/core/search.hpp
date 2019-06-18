#ifndef POAC_CORE_STROITE_CORE_SEARCH_HPP
#define POAC_CORE_STROITE_CORE_SEARCH_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem.hpp>

#include "../../../io/path.hpp"


namespace poac::core::stroite::core::search {
    bool is_cpp_file(const boost::filesystem::path& p) {
        namespace fs = boost::filesystem;
        return !fs::is_directory(p)
               && (p.extension().string() == ".cpp"
               || p.extension().string() == ".cxx"
               || p.extension().string() == ".cc"
               || p.extension().string() == ".cp");
    }

    std::vector<std::string>
    cpp(const boost::filesystem::path& base_dir) {
        namespace fs = boost::filesystem;
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
#endif // POAC_CORE_STROITE_CORE_SEARCH_HPP
