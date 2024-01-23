#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
inline constexpr StringRef testDesc = "Run the tests of a local package";

void testHelp() noexcept;
int testMain(std::span<const StringRef> args);
