#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef initDesc =
    "Create a new poac package in an existing directory";

void initHelp() noexcept;
int initMain(std::span<const StringRef> args);
