#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef helpDesc =
    "Displays help for a poac subcommand";

int helpMain(Vec<String>) noexcept;
void helpHelp() noexcept;
