#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef lintDesc = "Lint codes using cpplint";

int lintMain(const std::span<const StringRef>);
void lintHelp() noexcept;
