#ifndef POAC_UTIL_PRETTY_HPP
#define POAC_UTIL_PRETTY_HPP

#include <string>
#include <utility>


namespace poac::util::pretty {
    std::string to_time(const std::string& s) {
        double total_seconds = std::stod(s);
        if (total_seconds > 1.0) {
            int days = static_cast<int>( total_seconds / 60 / 60 / 24 );
            int hours = static_cast<int>( total_seconds / 60 / 60 ) % 24;
            int minutes = static_cast<int>( total_seconds / 60 ) % 60;
            int seconds = static_cast<int>( total_seconds ) % 60;

            std::string res;
            if (days > 0) res += std::to_string(days) + "d ";
            if (hours > 0) res += std::to_string(hours) + "h ";
            if (minutes > 0) res += std::to_string(minutes) + "m ";
            res += std::to_string(seconds) + "s";

            return res;
        }
        else {
            return s + "s";
        }
    }

    std::pair<float, std::string>
    to_byte(const float b) {
        // 1024
        const float kb = b / 1000;
        if (kb < 1) {
            return { b, "B" };
        }
        const float mb = kb / 1000;
        if (mb < 1) {
            return { kb, "KB" };
        }
        const float gb = mb / 1000;
        if (gb < 1) {
            return { mb, "MB" };
        }
        const float tb = gb / 1000;
        if (tb < 1) {
            return { gb, "GB" };
        }
        return { tb, "TB" };
    }
} // end namespace
#endif // !POAC_UTIL_PRETTY_HPP
