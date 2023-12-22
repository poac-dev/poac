#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef testDesc = "Run the tests of a local package";

int testMain(std::span<const StringRef>);
void testHelp() noexcept;
