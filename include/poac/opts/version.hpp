#ifndef POAC_OPTS_VERSION_HPP
#define POAC_OPTS_VERSION_HPP

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <poac/core/except.hpp>
#include <poac/io/config.hpp>

namespace poac::opts::version {
    const std::string summary = "Show the current poac version";
    const std::string options = "<Nothing>";

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::optional<io::config::Config>&&, std::vector<std::string>&&) noexcept {
        std::cout << POAC_VERSION << std::endl;
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_VERSION_HPP
