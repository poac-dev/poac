#ifndef POAC_CORE_EXCEPT_HPP
#define POAC_CORE_EXCEPT_HPP

#include <string>
#include <string_view>
#include <stdexcept>

namespace poac::core::except {
    class error : public std::invalid_argument {
    public:
        explicit error(const std::string_view __s)
            : invalid_argument(std::string(__s)) {}

        error() = delete;
        error(const error&) = default;
        error& operator=(const error&) = default;
        error(error&&) = default;
        error& operator=(error&&) = default;
        ~error() noexcept override = default;
    };
} // end namespace
#endif // !POAC_CORE_EXCEPT_HPP
