#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef buildDesc =
    "Compile a local package and all of its dependencies";

int build(Vec<String> args);
void buildHelp();
