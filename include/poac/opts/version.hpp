#ifndef POAC_OPTS_VERSION_HPP
#define POAC_OPTS_VERSION_HPP

#include <future>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <poac/core/except.hpp>
#include <poac/io/config.hpp>

namespace poac::opts::version {
    inline const clap::subcommand cli =
            clap::subcommand("update")
                .about("Show the current poac version")
            ;

    [[nodiscard]] std::optional<core::except::Error>
    exec(std::future<std::optional<io::config::Config>>&&, std::vector<std::string>&&) noexcept {
        std::cout << POAC_VERSION << std::endl;
        return std::nullopt;
    }
} // end namespace
#endif // !POAC_OPTS_VERSION_HPP
