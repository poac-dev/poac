#ifndef POAC_IO_FILE_PATH_HPP
#define POAC_IO_FILE_PATH_HPP

#include <iostream>
#include <string>

#include <boost/filesystem.hpp>


namespace poac::io::file::path {
    // Inspired by https://stackoverflow.com/questions/4891006/how-to-create-a-folder-in-the-home-directory
    std::string expand_user(std::string path);
    std::string expand_user(std::string path) {
        if (!path.empty() && path[0] == '~') {
            assert(path.size() == 1 or path[1] == '/');  // or other error handling
            const char* home = std::getenv("HOME");
            if (home || ((home = std::getenv("USERPROFILE")))) {
                path.replace(0, 1, home);
            }
            else {
                const char *hdrive = std::getenv("HOMEDRIVE"),
                        *hpath  = std::getenv("HOMEPATH");
                assert(hdrive);  // or other error handling
                assert(hpath);
                path.replace(0, 1, std::string(hdrive) + hpath);
            }
        }
        return path;
    }

    const boost::filesystem::path poac_state_dir(
            boost::filesystem::path(expand_user("~")) / ".poac"
    );
    const boost::filesystem::path poac_cache_dir(
            poac_state_dir / "cache"
    );
    const boost::filesystem::path poac_log_dir(
            poac_state_dir / "logs"
    );
    const boost::filesystem::path poac_token_dir(
            poac_state_dir / "token"
    );
    const boost::filesystem::path current_deps_dir(
            boost::filesystem::current_path() / "deps"
    );
    const boost::filesystem::path current_build_dir(
            boost::filesystem::current_path() / "_build"
    );
    const boost::filesystem::path current_build_bin_dir(
            current_build_dir / "bin"
    );
    const boost::filesystem::path current_build_include_dir(
            current_build_dir / "include"
    );
    const boost::filesystem::path current_build_lib_dir(
            current_build_dir / "lib"
    );

    bool validate_dir(const boost::filesystem::path& path) {
        namespace fs = boost::filesystem;
        return fs::exists(path) && fs::is_directory(path) && !fs::is_empty(path);
    }

    bool recursive_copy(const boost::filesystem::path &from, const boost::filesystem::path &dest) {
        namespace fs = boost::filesystem;
        try {
            // Check whether the function call is valid
            if (!fs::exists(from) || !fs::is_directory(from)) {
//                std::cerr << "Source directory " << source.string()
//                          << " does not exist or is not a directory." << '\n';
                return false;
            }
            if (!validate_dir(dest)) {
                // Create the destination directory
                if (!fs::create_directory(dest)) {
                    std::cerr << "Unable to create destination directory"
                              << dest.string() << '\n';
                    return false;
                }
            }
        }
        catch (fs::filesystem_error const & e) {
            std::cerr << e.what() << '\n';
            return false;
        }
        // Iterate through the source directory
        for (fs::directory_iterator file(from); file != fs::directory_iterator(); ++file) {
            try {
                fs::path current(file->path());
                if (fs::is_directory(current)) {
                    // Found directory: Recursion
                    if (!recursive_copy(current, dest / current.filename()))
                    { return false; }
                }
                else {
                    // Found file: Copy
                    fs::copy_file(current, dest / current.filename());
                }
            }
            catch(...) { /* Ignore error */ }
        }
        return true;
    }

//    void remove_all_files(const boost::filesystem::path& dir, const std::vector<std::string>& vs) {
//
//    }
} // end namespace
#endif // !POAC_IO_FILE_PATH_HPP
