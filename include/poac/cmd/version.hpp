#ifndef POAC_CMD_VERSION_HPP
#define POAC_CMD_VERSION_HPP

#include <iostream>
#include <optional>

#include <poac/core/except.hpp>

namespace poac::cmd::version {
    [[nodiscard]] std::optional<core::except::Error>
    exec() noexcept {
        std::cout << "poac " << POAC_VERSION << std::endl;
        return std::nullopt;
    }
} // end namespace

#endif // !POAC_CMD_VERSION_HPP
