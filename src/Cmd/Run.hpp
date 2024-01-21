#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef runDesc = "Build and execute src/main.cc";

void runHelp() noexcept;
int runMain(std::span<const StringRef> args);
