#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef testDesc = "Run the tests of a local package";

int testMain(std::span<const StringRef> args);
void testHelp() noexcept;
