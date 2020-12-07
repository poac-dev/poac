#ifndef POAC_CORE_VALIDATOR_HPP
#define POAC_CORE_VALIDATOR_HPP

// std
#include <filesystem>

// external
#include <fmt/core.h>
#include <mitama/result/result.hpp>

namespace poac::core::validator {
    mitama::result<void, std::string>
    require_config_exists(
        const std::filesystem::path& base = std::filesystem::current_path())
    noexcept
    {
        const auto config_path = base / "poac.toml";
        std::error_code ec{};
        if (std::filesystem::exists(config_path, ec)) {
            return mitama::success();
        }
        return mitama::failure(
            fmt::format(
                "required config file `{}` does not exist",
                config_path
            )
        );
    }
} // end namespace

#endif // POAC_CORE_VALIDATOR_HPP
