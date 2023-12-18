#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef newDesc = "Create a new poac project";

String getPoacToml(const String&);
bool verifyPackageName(StringRef);

int newMain(Vec<String>);
void newHelp();
