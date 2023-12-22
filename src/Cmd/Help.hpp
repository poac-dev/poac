#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef helpDesc =
    "Displays help for a poac subcommand";

int helpMain(std::span<const StringRef>) noexcept;
void helpHelp() noexcept;
