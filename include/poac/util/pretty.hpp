#ifndef POAC_UTIL_PRETTY_HPP
#define POAC_UTIL_PRETTY_HPP

// std
#include <cstdint>
#include <string>
#include <utility>

// external
#include <fmt/core.h>

namespace poac::util::pretty {
    std::string to_time(const std::string& s) {
        double total_seconds = std::stod(s);
        if (total_seconds <= 1.0) {
            return fmt::format("{:.2f}s", total_seconds);
        }

        std::string res;
        const auto total_secs = static_cast<std::uint_fast64_t>(total_seconds);
        if (const auto days = total_secs / 60 / 60 / 24; days > 0) {
            res += std::to_string(days) + "d ";
        }
        if (const auto hours = (total_secs / 60 / 60) % 24; hours > 0) {
            res += std::to_string(hours) + "h ";
        }
        if (const auto minutes = (total_secs / 60) % 60; minutes > 0) {
            res += std::to_string(minutes) + "m ";
        }
        const auto seconds = total_secs % 60;
        res += std::to_string(seconds) + "s";
        return res;
    }

    inline const std::vector<std::string> size_suffixes = {
        "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"
    };

    std::string
    to_byte(double bytes) {
        int index = 0;
        for (; bytes >= 1000.0; bytes /= 1024.0, ++index);
        return fmt::format("{:.2f}{}", bytes, size_suffixes.at(index));
    }

    // If string size is over specified number of characters and it can be clipped,
    //  display an ellipsis (...).
    inline std::string
    clip_string(const std::string& s, const std::size_t& n) {
        return s.size() <= n ? s : s.substr(0, n) + "...";
    }
} // end namespace
#endif // !POAC_UTIL_PRETTY_HPP
