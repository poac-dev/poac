#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef runDesc = "Build and execute src/main.cc";

int runMain(const std::span<const StringRef>);
void runHelp() noexcept;
