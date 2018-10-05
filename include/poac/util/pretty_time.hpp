#ifndef POAC_UTIL_PRETTY_TIME_HPP
#define POAC_UTIL_PRETTY_TIME_HPP

#include <string>


namespace poac::util::pretty_time {
    std::string to(const std::string& s) {
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
} // end namespace
#endif // !POAC_UTIL_PRETTY_TIME_HPP
