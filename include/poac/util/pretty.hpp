#ifndef POAC_UTIL_PRETTY_HPP
#define POAC_UTIL_PRETTY_HPP

// std
#include <cmath>
#include <cstdint>
#include <chrono>
#include <string>
#include <utility>
#include <vector>

// external
#include <boost/algorithm/string.hpp>
#include <fmt/core.h>

namespace poac::util::pretty {
    std::string to_time(const double& total_seconds) {
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

    inline std::string to_time(const std::chrono::seconds& s) {
        return to_time(s.count());
    }

    inline std::string to_time(const std::string& s) {
        return to_time(std::stod(s));
    }

    inline const std::vector<std::string> size_suffixes = {
        "B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"
    };

    std::string to_byte(double bytes) {
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

    // This function does not break long words and break on hyphens.
    // This assumes that space size is one (` `), not two (`  `).
    // textwrap(s, 15) =>
    // This function
    // does not break
    // long words and
    // break on
    // hyphens.
    std::vector<std::string>
    textwrap(const std::string& text, std::size_t width = 70) {
        std::vector<std::string> split_texts;
        boost::split(split_texts, text, boost::is_space());

        std::vector<std::string> wrapped_texts;
        std::string consuming_text;
        for (const auto& st : split_texts) {
            if (consuming_text.size() + st.size() < width) {
                consuming_text += consuming_text.empty() ? st : " " + st; // assumes space size is one
            } else {
                wrapped_texts.emplace_back(consuming_text);
                consuming_text = st;
            }
        }
        if (!consuming_text.empty()) {
            wrapped_texts.emplace_back(consuming_text);
        }
        return wrapped_texts;
    }
} // end namespace
#endif // !POAC_UTIL_PRETTY_HPP
