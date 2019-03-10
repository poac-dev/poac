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

    std::pair<std::string, std::string>
    to_byte(const unsigned long b) {
        const unsigned long kb = b / 1024;
        if (kb < 1) {
            return { std::to_string(b), "B" };
        }
        const unsigned long mb = kb / 1024;
        if (mb < 1) {
            return { std::to_string(kb), "KB" };
        }
        const unsigned long gb = mb / 1024;
        if (gb < 1) {
            return { std::to_string(mb), "MB" };
        }
        const unsigned long tb = gb / 1024;
        if (tb < 1) {
            return { std::to_string(gb), "GB" };
        }
        return { std::to_string(tb), "TB" };
    }
} // end namespace
#endif // !POAC_UTIL_PRETTY_HPP
