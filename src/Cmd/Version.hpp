#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef versionDesc = "Show version information";

int versionMain(std::span<const StringRef> args) noexcept;
void versionHelp() noexcept;
