#ifndef POAC_IO_PATH_HPP
#define POAC_IO_PATH_HPP

// std
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <filesystem>
#include <string>
#include <optional>

// external
#include <fmt/core.h>
#include <boost/predef.h>
#include <mitama/result/result.hpp>

// internal
#include <poac/core/except.hpp>

namespace std::filesystem {
    inline namespace path_literals {
        inline std::filesystem::path
        operator "" _path(const char* str, std::size_t) noexcept {
            return std::filesystem::path(str);
        }
    }
}

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
            return home.value();
        } else {
            const auto home_drive = dupenv("HOMEDRIVE");
            const auto home_path = dupenv("HOMEPATH");
            if (home_drive && home_path) {
                return home_drive.value() + home_path.value();
            }
            throw core::except::error(
                "could not read environment variable"
            );
        }
    }

    inline const std::filesystem::path root(expand_user() / std::filesystem::path(".poac"));
    inline const std::filesystem::path cache_dir(root / "cache");
    inline const std::filesystem::path archive_dir(cache_dir / "archive");

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

    mitama::result<void, std::string>
    validate_dir(const std::filesystem::path& path) noexcept {
        namespace fs = std::filesystem;
        std::error_code ec{}; // This is to use for noexcept optimization

        const bool exists = fs::exists(path, ec);
        if (exists && !fs::is_directory(path, ec)) {
            return mitama::failure(
                fmt::format(
                    "The `{}` directory could not be created because the same name file exists",
                    path.string()
                )
            );
        } else if (exists && !fs::is_empty(path, ec)) {
            return mitama::failure(
                fmt::format(
                    "The `{}` directory already exists and is not empty",
                    path.string()
                )
            );
        }
        return mitama::success();
    }

    bool copy_recursive(const std::filesystem::path& from, const std::filesystem::path& dest) noexcept {
        try {
            std::filesystem::copy(from, dest, std::filesystem::copy_options::recursive);
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
#endif // !POAC_IO_PATH_HPP
