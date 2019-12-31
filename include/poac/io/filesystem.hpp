#ifndef POAC_IO_FILESYSTEM_HPP
#define POAC_IO_FILESYSTEM_HPP

#include <cstdlib>
#include <ctime>
#include <chrono>
#include <string>
#include <optional>

#include <boost/predef.h>

#include <poac/core/except.hpp>

#include <filesystem>
#include <system_error>
namespace poac::io::filesystem {
    using namespace std::filesystem;
}

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

    inline bool validate_dir(const io::filesystem::path& path) {
        namespace fs = io::filesystem;
        return fs::exists(path) && fs::is_directory(path) && !fs::is_empty(path);
    }

    bool copy_recursive(const io::filesystem::path& from, const io::filesystem::path& dest) noexcept {
        try {
            io::filesystem::copy(from, dest, io::filesystem::copy_options::recursive);
        } catch (...) {
            return false;
        }
        return true;
    }

    inline std::string
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
