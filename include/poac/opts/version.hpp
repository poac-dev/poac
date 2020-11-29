#ifndef POAC_OPTS_VERSION_HPP
#define POAC_OPTS_VERSION_HPP

#include <iostream>
#include <optional>

#include <poac/core/except.hpp>

namespace poac::opts::version {
    [[nodiscard]] std::optional<core::except::Error>
    exec() noexcept {
        std::cout << "poac " << POAC_VERSION << std::endl;
        return std::nullopt;
    }
} // end namespace

#endif // !POAC_OPTS_VERSION_HPP
