#ifndef POAC_UTIL_PRETTY_HPP
#define POAC_UTIL_PRETTY_HPP

#include <cstdint>
#include <string>
#include <utility>

namespace poac::util::pretty {
    std::string to_time(const std::string& s) {
        double total_seconds = std::stod(s);
        if (total_seconds > 1.0) {
            std::string res;

            const auto total_secs = static_cast<std::uint_fast64_t>(total_seconds);
            const auto days = static_cast<std::uint_fast8_t>(total_secs / 60 / 60 / 24);
            if (days > 0) {
                res += std::to_string(days) + "d ";
            }
            const auto hours = static_cast<std::uint_fast8_t>((total_secs / 60 / 60) % 24);
            if (hours > 0) {
                res += std::to_string(hours) + "h ";
            }
            const auto minutes = static_cast<std::uint_fast8_t>((total_secs / 60) % 60);
            if (minutes > 0) {
                res += std::to_string(minutes) + "m ";
            }
            const auto seconds = static_cast<std::uint_fast8_t>(total_secs % 60);
            res += std::to_string(seconds) + "s";

            return res;
        }
        else {
            return s + "s";
        }
    }

    std::pair<float, std::string>
    to_byte(const double b) {
        // 1024
        const double kb = b / 1000.0;
        if (kb < 1) {
            return { b, "B" };
        }
        const double mb = kb / 1000.0;
        if (mb < 1) {
            return { kb, "KB" };
        }
        const double gb = mb / 1000.0;
        if (gb < 1) {
            return { mb, "MB" };
        }
        const double tb = gb / 1000.0;
        if (tb < 1) {
            return { gb, "GB" };
        }
        return { tb, "TB" };
    }

    // If string size is over specified number of characters and it can be clipped,
    //  display an ellipsis (...).
    std::string clip_string(const std::string& s, const std::size_t& n) {
        if (s.size() <= n) {
            return s;
        } else {
            return s.substr(0, n) + "...";
        }
    }
} // end namespace
#endif // !POAC_UTIL_PRETTY_HPP
