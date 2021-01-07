#ifndef POAC_UTIL_EXECUTION_TIME_HPP
#define POAC_UTIL_EXECUTION_TIME_HPP

#include <chrono>

namespace poac::util {
    class execution_time_t {
        std::chrono::high_resolution_clock::time_point start;

    public:
        execution_time_t() : start(std::chrono::high_resolution_clock::now()) {}

        std::chrono::seconds measure() {
            return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - start
            );
        }
    };
} // end namespace

#endif // POAC_UTIL_EXECUTION_TIME_HPP
