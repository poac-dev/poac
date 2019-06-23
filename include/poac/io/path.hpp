#ifndef POAC_IO_PATH_HPP
#define POAC_IO_PATH_HPP

#include <fstream>
#include <string>
#include <optional>

#include <boost/predef.h>
#include <boost/filesystem.hpp>

#include "../core/except.hpp"

namespace poac::io::path {
    std::optional<std::string>
    dupenv(const std::string& sv) {
#if BOOST_COMP_MSVC
        std::string env;
        std::size_t len;
        if (std::_dupenv_s(env.c_str(), &len, sv.c_str())) {
            return std::nullopt;
        }
        else {
            return env;
        }
#else
        if (const char* env = std::getenv(sv.c_str())) {
            return env;
        }
        else {
            return std::nullopt;
        }
#endif
    }

    // Inspired by https://stackoverflow.com/q/4891006
    // Expand ~ to user home directory.
    std::string expand_user() {
        namespace except = core::except;

        auto home = dupenv("HOME");
        if (home || (home = dupenv("USERPROFILE"))) {
            return *home;
        }
        else {
            const auto hdrive = dupenv("HOMEDRIVE");
            const auto hpath = dupenv("HOMEPATH");
            if (hdrive && hpath) {
                return hdrive.value() + hpath.value();
            }
        }
        throw except::error(
                except::msg::could_not_read("environment variable"));
    }

    inline namespace path_literals {
        inline boost::filesystem::path
        operator "" _path(const char* str, std::size_t) noexcept {
            return boost::filesystem::path(str);
        }
    }

    const boost::filesystem::path poac_dir{
        expand_user() / ".poac"_path
    };

    const boost::filesystem::path poac_cache_dir{
        poac_dir / "cache"
    };

    const boost::filesystem::path poac_log_dir{
        poac_dir / "logs"
    };

    const boost::filesystem::path current_deps_dir{
        boost::filesystem::current_path() / "deps"
    };

    const boost::filesystem::path current_build_dir{
        boost::filesystem::current_path() / "_build"
    };

    const boost::filesystem::path current_build_cache_dir{
        current_build_dir / "_cache"
    };

    const boost::filesystem::path current_build_cache_obj_dir{
        current_build_cache_dir / "obj"
    };

    // timestamp
    const boost::filesystem::path current_build_cache_ts_dir{
        current_build_cache_dir / "_ts"
    };

    const boost::filesystem::path current_build_bin_dir{
        current_build_dir / "bin"
    };

    const boost::filesystem::path current_build_lib_dir{
        current_build_dir / "lib"
    };

    const boost::filesystem::path current_build_test_dir{
        current_build_dir / "test"
    };

    const boost::filesystem::path current_build_test_bin_dir{
        current_build_test_dir / "bin"
    };

    const boost::filesystem::path current_build_test_report_dir{
        current_build_test_dir / "report"
    };

    bool validate_dir(const boost::filesystem::path& path) {
        namespace fs = boost::filesystem;
        return fs::exists(path) && fs::is_directory(path) && !fs::is_empty(path);
    }

    bool recursive_copy(
        const boost::filesystem::path& from,
        const boost::filesystem::path& dest
    ) {
        namespace fs = boost::filesystem;

        // Does the copy source exist?
        if (!fs::exists(from) || !fs::is_directory(from)) {
            return false;
        }
        // Does the copy destination exist?
        if (!validate_dir(dest) && !fs::create_directories(dest)) {
            return false; // Unable to create destination directory
        }
        // Iterate through the source directory
        for (fs::directory_iterator file(from); file != fs::directory_iterator(); ++file) {
            fs::path current(file->path());
            if (fs::is_directory(current)) {
                // Found directory: Recursion
                if (recursive_copy(current, dest / current.filename())) {
                    return false;
                }
            }
            else {
                // Found file: Copy
                boost::system::error_code error;
                fs::copy_file(current, dest / current.filename(), error);
                if (error) {
                    return false;
                }
            }
        }
        return true;
    }

    void write_to_file(std::ofstream& ofs, const std::string& fname, const std::string& text) {
        ofs.open(fname);
        if (ofs.is_open()) {
            ofs << text;
        }
        ofs.close();
        ofs.clear();
    }
} // end namespace
#endif // !POAC_IO_PATH_HPP
