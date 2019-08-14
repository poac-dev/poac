#ifndef POAC_IO_PATH_HPP
#define POAC_IO_PATH_HPP

#include <cstdlib>
#include <ctime>
#include <chrono>
#include <fstream>
#include <string>
#include <optional>

#include <boost/predef.h>

#include <poac/core/except.hpp>

#if BOOST_OS_LINUX
#  if __has_include(<filesystem>)
#    include <filesystem>
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
namespace std::filesystem { // poac::io::path
    using namespace std::experimental::filesystem;
}
#  else
#    error "Could not find a filesystem header"
#  endif
#  include <system_error>
#else
#  include <boost/filesystem.hpp>
#  include <boost/system/system_error.hpp>
namespace std::filesystem { // poac::io::path
    using namespace boost::filesystem;
}
#endif

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
            throw core::except::error(
                    core::except::msg::could_not_read("environment variable"));
        }
    }

    inline namespace path_literals {
        inline std::filesystem::path
        operator "" _path(const char* str, std::size_t) noexcept {
            return std::filesystem::path(str);
        }
    }

    inline const std::filesystem::path poac_dir(expand_user() / ".poac"_path);
    inline const std::filesystem::path poac_cache_dir(poac_dir / "cache");
    inline const std::filesystem::path poac_log_dir(poac_dir / "logs");

    inline const std::filesystem::path current(std::filesystem::current_path());
    inline const std::filesystem::path current_deps_dir(current / "deps");
    inline const std::filesystem::path current_build_dir(current / "_build");
    inline const std::filesystem::path current_build_cache_dir(current_build_dir / "_cache");
    inline const std::filesystem::path current_build_cache_obj_dir(current_build_cache_dir / "obj");
    inline const std::filesystem::path current_build_cache_ts_dir(current_build_cache_dir / "_ts");
    inline const std::filesystem::path current_build_bin_dir(current_build_dir / "bin");
    inline const std::filesystem::path current_build_lib_dir(current_build_dir / "lib");
    inline const std::filesystem::path current_build_test_dir(current_build_dir / "test");
    inline const std::filesystem::path current_build_test_bin_dir(current_build_test_dir / "bin");
    inline const std::filesystem::path current_build_test_report_dir(current_build_test_dir / "report");

    bool validate_dir(const std::filesystem::path& path) {
        namespace fs = std::filesystem;
        return fs::exists(path) && fs::is_directory(path) && !fs::is_empty(path);
    }

#if !BOOST_OS_MACOS
    bool copy(const std::filesystem::path& from, const std::filesystem::path& dest) noexcept {
        namespace fs = std::filesystem;
        try {
            fs::copy(from, dest, fs::copy_options::recursive);
        } catch (...) {
            return false;
        }
    }
#else
    bool copy(const std::filesystem::path& from, const std::filesystem::path& dest) {
        namespace fs = std::filesystem;

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
            const fs::path cur(file->path());
            if (fs::is_directory(cur)) {
                // Found directory: Recursion
                if (path::copy(cur, dest / cur.filename())) {
                    return false;
                }
            } else {
                // Found file: Copy
                try {
                    fs::copy_file(cur, dest / cur.filename());
                } catch (...) {
                    return false;
                }
            }
        }
        return true;
    }
#endif

    std::string
    time_to_string(const std::time_t& time) {
        return std::to_string(time);
    }
    template <typename Clock, typename Duration>
    std::string
    time_to_string(const std::chrono::time_point<Clock, Duration>& time) {
        // FIXME: https://developercommunity.visualstudio.com/content/problem/251213/stdfilesystemfile-time-type-does-not-allow-easy-co.html
        const auto sys_time = std::chrono::time_point_cast<std::chrono::system_clock>(time);
        return time_to_string(std::chrono::system_clock::to_time_t(sys_time));
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
