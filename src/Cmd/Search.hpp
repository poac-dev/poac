#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef searchDesc =
    "Search for packages in the registry";

int searchMain(std::span<const StringRef>);
void searchHelp() noexcept;
