#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef initDesc =
    "Create a new poac package in an existing directory";

int initMain(std::span<const StringRef>);
void initHelp() noexcept;
