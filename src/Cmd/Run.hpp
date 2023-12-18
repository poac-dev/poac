#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef runDesc = "Build and execute src/main.cc";

int runCmd(Vec<String> args);
void runHelp();
