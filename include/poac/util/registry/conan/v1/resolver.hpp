#pragma once

// internal
#include "poac/core/resolver/resolve.hpp"
#include "poac/util/rustify.hpp"

namespace poac::util::registry::conan::v1::resolver {

using ConanNotFound = Error<"conan is not found">;
using ConanIsNotV1 =
    Error<"your conan is not v1. conan-v1 registry needs conan v1.">;

Fn check_conan_command()->Result<void>;

Fn install_conan_generator()->Result<void>;

Fn format_conan_requires(const Vec<core::resolver::resolve::Package>& packages)
    ->String;

Fn get_conan_config()->String;

Fn install_conan_packages()->Result<void>;

Fn fetch_conan_packages(const Vec<core::resolver::resolve::Package>& packages
) noexcept -> Result<void>;

Fn is_conan(const core::resolver::resolve::Package& package) noexcept -> bool;

} // namespace poac::util::registry::conan::v1::resolver
