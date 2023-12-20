#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef newDesc = "Create a new poac project";

String getPoacToml(const String&) noexcept;
bool verifyPackageName(StringRef) noexcept;

int newMain(Vec<String>);
void newHelp() noexcept;
