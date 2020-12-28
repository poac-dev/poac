#ifndef POAC_IO_PATH_HPP
#define POAC_IO_PATH_HPP

// std
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <filesystem>
#include <string>

// external
#include <fmt/core.h>
#include <boost/predef.h>
#include <mitama/result/result.hpp>

// internal
#include <poac/core/except.hpp>

namespace poac::io::path {
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
