#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef newDesc = "Create a new poac project";

int newCmd(Vec<String> args);
void newHelp();
