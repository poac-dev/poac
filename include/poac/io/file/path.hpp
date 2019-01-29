#ifndef POAC_IO_FILE_PATH_HPP
#define POAC_IO_FILE_PATH_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "../../util/command.hpp"


namespace poac::io::file::path {
    // Inspired by https://stackoverflow.com/q/4891006
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
    const boost::filesystem::path current_build_cache_dir(
            current_build_dir / "_cache"
    );
    const boost::filesystem::path current_build_cache_obj_dir(
            current_build_cache_dir / "obj"
    );
    const boost::filesystem::path current_build_cache_hash_dir(
            current_build_cache_dir / "_hash"
    );
    const boost::filesystem::path current_build_bin_dir(
            current_build_dir / "bin"
    );
    const boost::filesystem::path current_build_lib_dir(
            current_build_dir / "lib"
    );
    const boost::filesystem::path current_build_test_dir(
            current_build_dir / "test"
    );
    const boost::filesystem::path current_build_test_bin_dir(
            current_build_test_dir / "bin"
    );
    const boost::filesystem::path current_build_test_report_dir(
            current_build_test_dir / "report"
    );

    bool validate_dir(const boost::filesystem::path& path) {
        namespace fs = boost::filesystem;
        return fs::exists(path) && fs::is_directory(path) && !fs::is_empty(path);
    }

    bool recursive_copy(
        const boost::filesystem::path &from,
        const boost::filesystem::path &dest )
    {
        namespace fs = boost::filesystem;

        // Does the copy source exist?
        if (!fs::exists(from) || !fs::is_directory(from)) {
//            std::cerr << "Could not validate `from` dir" << std::endl;
            return EXIT_FAILURE;
        }
        // Does the copy destination exist?
        if (!validate_dir(dest) && !fs::create_directories(dest)) {
//            std::cerr << "Could not validate `dest` dir" << std::endl;
            return EXIT_FAILURE; // Unable to create destination directory
        }

        // Iterate through the source directory
        for (fs::directory_iterator file(from); file != fs::directory_iterator(); ++file) {
            fs::path current(file->path());
            if (fs::is_directory(current)) {
                // Found directory: Recursion
                if (recursive_copy(current, dest / current.filename()))
                    return EXIT_FAILURE;
            }
            else {
                // Found file: Copy
                boost::system::error_code err;
                fs::copy_file(current, dest / current.filename(), err);
                if (err) {
//                    std::cerr << err.message() << std::endl;
                    return EXIT_FAILURE;
                }
            }
        }
        return EXIT_SUCCESS;
    }

    std::optional<std::string> read_file(const boost::filesystem::path& path) {
        if (!boost::filesystem::exists(path)) {
            return std::nullopt;
        }
        else if (std::ifstream ifs(path.string()); !ifs.fail()) {
            std::istreambuf_iterator<char> it(ifs);
            std::istreambuf_iterator<char> last;
            return std::string(it, last);
        }
        else {
            return std::nullopt;
        }
    }

    void write_to_file(std::ofstream& ofs, const std::string& fname, const std::string& text) {
        ofs.open(fname);
        if (ofs.is_open()) ofs << text;
        ofs.close();
        ofs.clear();
    }

    std::vector<std::string>
    split(const std::string& raw, const std::string& delim) {
        using boost::algorithm::token_compress_on;
        using boost::is_any_of;

        std::vector<std::string> ret;
        boost::split(ret, raw, is_any_of(delim), token_compress_on);
        return ret;
    }

    boost::filesystem::path create_temp() {
        const std::string temp = *(util::command("mktemp -d").exec());
        const std::string temp_path(temp, 0, temp.size()-1); // delete \n
        return temp_path;
    }

    void remove_matched_files(const boost::filesystem::path& p, std::regex r) {
        namespace fs = boost::filesystem;

        fs::directory_iterator end_itr; // Default ctor yields past-the-end
        for (fs::directory_iterator i(p); i != end_itr; ++i) {
            // Skip if not a file
            if (!fs::is_regular_file(i->status())) {
                continue;
            }
            // Skip if no match
            if (!std::regex_match(i->path().filename().string(), r)) {
                continue;
            }
            // File matches, delete it
            fs::remove_all(i->path());
        }
    }
} // end namespace
#endif // !POAC_IO_FILE_PATH_HPP
