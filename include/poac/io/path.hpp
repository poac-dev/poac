#ifndef POAC_IO_PATH_HPP
#define POAC_IO_PATH_HPP

// std
#include <ctime>
#include <chrono>
#include <string>

namespace poac::io::path {
    template <class Clock, class Duration>
    inline std::string
    time_to_string(const std::chrono::time_point<Clock, Duration>& time) {
        return std::to_string(
            std::chrono::duration_cast<std::chrono::seconds>(
                time.time_since_epoch()
            ).count()
        );
    }
} // end namespace
#endif // !POAC_IO_PATH_HPP
