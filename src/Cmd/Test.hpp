#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef testDesc = "Run the tests of a local package";

int testCmd(Vec<String> args);
void testHelp();
