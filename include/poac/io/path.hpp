#ifndef POAC_IO_PATH_HPP
#define POAC_IO_PATH_HPP

#include <cstdlib>
#include <fstream>
#include <string>
#include <optional>

#include <boost/predef.h>
#include <boost/filesystem.hpp>

#include <poac/core/except.hpp>

namespace poac::io::path {
    std::optional<std::string>
    dupenv(const std::string& name) {
#if BOOST_COMP_MSVC
        char* env;
        std::size_t len;
        if (_dupenv_s(&env, &len, name.c_str())) {
            return std::nullopt;
        } else {
            std::string env_s(env);
            std::free(env);
            return env_s;
        }
#else
        if (const char* env = std::getenv(name.c_str())) {
            return env;
        } else {
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
            throw except::error(
                    except::msg::could_not_read("environment variable"));
        }
    }

    inline namespace path_literals {
        inline boost::filesystem::path
        operator "" _path(const char* str, std::size_t) noexcept {
            return boost::filesystem::path(str);
        }
    }

    inline const boost::filesystem::path poac_dir(expand_user() / ".poac"_path);
    inline const boost::filesystem::path poac_cache_dir(poac_dir / "cache");
    inline const boost::filesystem::path poac_log_dir(poac_dir / "logs");

    inline boost::system::error_code ec{};
    inline const boost::filesystem::path current(boost::filesystem::current_path(ec));
    inline const boost::filesystem::path current_deps_dir(current / "deps");
    inline const boost::filesystem::path current_build_dir(current / "_build");
    inline const boost::filesystem::path current_build_cache_dir(current_build_dir / "_cache");
    inline const boost::filesystem::path current_build_cache_obj_dir(current_build_cache_dir / "obj");
    inline const boost::filesystem::path current_build_cache_ts_dir(current_build_cache_dir / "_ts");
    inline const boost::filesystem::path current_build_bin_dir(current_build_dir / "bin");
    inline const boost::filesystem::path current_build_lib_dir(current_build_dir / "lib");
    inline const boost::filesystem::path current_build_test_dir(current_build_dir / "test");
    inline const boost::filesystem::path current_build_test_bin_dir(current_build_test_dir / "bin");
    inline const boost::filesystem::path current_build_test_report_dir(current_build_test_dir / "report");

    bool validate_dir(const boost::filesystem::path& path) {
        namespace fs = boost::filesystem;
        return fs::exists(path) && fs::is_directory(path) && !fs::is_empty(path);
    }

    bool recursive_copy(const boost::filesystem::path& from, const boost::filesystem::path& dest) {
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
            } else {
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
