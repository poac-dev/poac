#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
inline constexpr StringRef searchDesc = "Search for packages in the registry";

void searchHelp() noexcept;
int searchMain(std::span<const StringRef> args);
