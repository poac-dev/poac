#ifndef POAC_IO_PATH_HPP
#define POAC_IO_PATH_HPP

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <optional>

#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#include "../core/except.hpp"
#include "../util/shell.hpp"


namespace poac::io::path {
    // Inspired by https://stackoverflow.com/q/4891006
    std::string expand_user(std::string path) {
        namespace except = core::except;

        if (!path.empty() && path[0] == '~') {
            assert(path.size() == 1 || path[1] == '/');
            const char* home = std::getenv("HOME");
            if (home || ((home = std::getenv("USERPROFILE")))) {
                path.replace(0, 1, home);
            }
            else {
                if (const char* hdrive = std::getenv("HOMEDRIVE")) {
                    if (const char* hpath = std::getenv("HOMEPATH")) {
                        path.replace(0, 1, std::string(hdrive) + hpath);
                    }
                    else {
                        throw except::error(
                                except::msg::could_not_read("environment variable HOMEPATH"));
                    }
                }
                else {
                    throw except::error(
                            except::msg::could_not_read("environment variable HOMEDRIVE"));
                }
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
    const boost::filesystem::path current_build_cache_hash_dir( // FIXME: hashでなく，timestamp
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
                boost::system::error_code error;
                fs::copy_file(current, dest / current.filename(), error);
                if (error) {
//                    std::cerr << err.message() << std::endl;
                    return EXIT_FAILURE;
                }
            }
        }
        return EXIT_SUCCESS;
    }

    void write_to_file(std::ofstream& ofs, const std::string& fname, const std::string& text) {
        ofs.open(fname);
        if (ofs.is_open()) ofs << text;
        ofs.close();
        ofs.clear();
    }
} // end namespace
#endif // !POAC_IO_PATH_HPP
