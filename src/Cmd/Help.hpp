#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef helpDesc =
    "Prints this message or the help of the given subcommand(s)";

int helpMain(Vec<String>) noexcept;
void helpHelp() noexcept;
