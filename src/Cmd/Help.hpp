#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef helpDesc =
    "Displays help for a poac subcommand";

void helpHelp() noexcept;
int helpMain(std::span<const StringRef> args) noexcept;
