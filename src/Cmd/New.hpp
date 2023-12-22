#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef newDesc = "Create a new poac project";

String getPoacToml(const String&) noexcept;
bool verifyPackageName(StringRef) noexcept;

int newMain(std::span<const StringRef>);
void newHelp() noexcept;
