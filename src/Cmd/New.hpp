#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
inline constexpr StringRef newDesc = "Create a new poac project";

String createPoacToml(StringRef projectName) noexcept;

void newHelp() noexcept;
int newMain(std::span<const StringRef> args);
