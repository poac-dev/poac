#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
inline constexpr StringRef versionDesc = "Show version information";

void versionHelp() noexcept;
int versionMain(std::span<const StringRef> args) noexcept;
