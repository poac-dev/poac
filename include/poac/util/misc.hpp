// Miscellaneous utility
#ifndef POAC_UTIL_MISC_HPP
#define POAC_UTIL_MISC_HPP

// std
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

// external
#include <boost/algorithm/string.hpp>
#include <boost/predef.h>
#include <mitama/result/result.hpp>

namespace poac::util::misc {
    inline namespace path_literals {
        inline std::filesystem::path
        operator ""_path(const char* str, std::size_t) {
            return std::filesystem::path(str);
        }
    }

    std::vector<std::string>
    split(const std::string& raw, const std::string& delim) {
        using boost::algorithm::token_compress_on;
        using boost::is_any_of;

        std::vector<std::string> ret;
        boost::split(ret, raw, is_any_of(delim), token_compress_on);
        return ret;
    }

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
    [[nodiscard]] mitama::result<std::filesystem::path, std::string>
    expand_user() {
        auto home = dupenv("HOME");
        if (home || (home = dupenv("USERPROFILE"))) {
            return mitama::success(home.value());
        } else {
            const auto home_drive = dupenv("HOMEDRIVE");
            const auto home_path = dupenv("HOMEPATH");
            if (home_drive && home_path) {
                return mitama::success(home_drive.value() + home_path.value());
            }
            return mitama::failure("could not get home directory");
        }
    }
} // end namespace
#endif // POAC_UTIL_MISC_HPP
