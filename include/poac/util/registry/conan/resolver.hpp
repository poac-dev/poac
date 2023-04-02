#pragma once

// internal
#include "poac/core/resolver/resolve.hpp"
#include "poac/util/rustify.hpp"

namespace poac::util::registry::conan::resolver {

using ConanNotFound = Error<"conan is not found">;

Result<void> check_conan_command();

Result<void> install_conan_generator();

String format_conan_requires(
    const Vec<poac::core::resolver::resolve::Package>& packages
);

String get_conan_config();

Result<void> install_conan_packages();

Result<void>
fetch_conan_packages(const Vec<poac::core::resolver::resolve::Package>& packages
) noexcept;

} // namespace poac::util::registry::conan::resolver
