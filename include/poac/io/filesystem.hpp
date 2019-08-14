#ifndef POAC_IO_FILESYSTEM_HPP
#define POAC_IO_FILESYSTEM_HPP

#include <cstdlib>
#include <ctime>
#include <chrono>
#include <string>
#include <optional>

#include <boost/predef.h>

#include <poac/core/except.hpp>

#if BOOST_OS_LINUX
#  if __has_include(<filesystem>)
#    include <filesystem>
namespace poac::io::filesystem {
    using namespace std::filesystem;
}
#  elif __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
namespace poac::io::filesystem {
    using namespace std::experimental::filesystem;
}
#  else
#    error "Could not find a filesystem header"
#  endif
#  include <system_error>
#else
#  include <boost/filesystem.hpp>
#  include <boost/system/system_error.hpp>
namespace poac::io::filesystem {
    using namespace boost::filesystem;
}
#endif
// In macOS 10.14 or earlier, when std::filesystem is used,
//   it is diverted to an unimplemented filesystem header so
//   do not expand std::filesystem namespace to boost::filesystem,
//   expand to poac::io::path namespace.
// error: 'path' is unavailable: introduced in macOS 10.15
// /Applications/Xcode-11.0.app/Contents/Developer/Toolchains/
// XcodeDefault.xctoolchain/usr/bin/../include/c++/v1/filesystem:739:24:
// note: 'path' has been explicitly marked unavailable here

namespace poac::io::filesystem {
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
        inline io::filesystem::path
        operator "" _path(const char* str, std::size_t) noexcept {
            return io::filesystem::path(str);
        }
    }

    inline const io::filesystem::path poac_dir(expand_user() / ".poac"_path);
    inline const io::filesystem::path poac_cache_dir(poac_dir / "cache");
    inline const io::filesystem::path poac_log_dir(poac_dir / "logs");

    inline const io::filesystem::path current(io::filesystem::current_path());
    inline const io::filesystem::path current_deps_dir(current / "deps");
    inline const io::filesystem::path current_build_dir(current / "_build");
    inline const io::filesystem::path current_build_cache_dir(current_build_dir / "_cache");
    inline const io::filesystem::path current_build_cache_obj_dir(current_build_cache_dir / "obj");
    inline const io::filesystem::path current_build_cache_ts_dir(current_build_cache_dir / "_ts");
    inline const io::filesystem::path current_build_bin_dir(current_build_dir / "bin");
    inline const io::filesystem::path current_build_lib_dir(current_build_dir / "lib");
    inline const io::filesystem::path current_build_test_dir(current_build_dir / "test");
    inline const io::filesystem::path current_build_test_bin_dir(current_build_test_dir / "bin");
    inline const io::filesystem::path current_build_test_report_dir(current_build_test_dir / "report");

    bool validate_dir(const io::filesystem::path& path) {
        namespace fs = io::filesystem;
        return fs::exists(path) && fs::is_directory(path) && !fs::is_empty(path);
    }

#if BOOST_OS_LINUX
    bool copy_recursive(const io::filesystem::path& from, const io::filesystem::path& dest) noexcept {
        try {
            io::filesystem::copy(from, dest, io::filesystem::copy_options::recursive);
        } catch (...) {
            return false;
        }
    }
#else
    bool copy_recursive(const io::filesystem::path& from, const io::filesystem::path& dest) {
        // Does the copy source exist?
        if (!io::filesystem::exists(from) || !io::filesystem::is_directory(from)) {
            return false;
        }
        // Does the copy destination exist?
        if (!validate_dir(dest) && !io::filesystem::create_directories(dest)) {
            return false; // Unable to create destination directory
        }
        // Iterate through the source directory
        for (io::filesystem::directory_iterator file(from); file != io::filesystem::directory_iterator(); ++file) {
            const io::filesystem::path cur(file->path());
            if (io::filesystem::is_directory(cur)) {
                // Found directory: Recursion
                if (filesystem::copy_recursive(cur, dest / cur.filename())) {
                    return false;
                }
            } else {
                // Found file: Copy
                try {
                    io::filesystem::copy_file(cur, dest / cur.filename());
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
        const auto sec = std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch());
        const std::time_t t = sec.count();
        return time_to_string(t);
    }
} // end namespace
#endif // !POAC_IO_FILESYSTEM_HPP
