#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef testDesc = "Run the tests of a local package";

int testMain(Vec<String>);
void testHelp();
