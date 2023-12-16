#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef runDesc = "Build and execute src/main.cc";

int run(Vec<String> args);
void runHelp();
