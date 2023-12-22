#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef versionDesc = "Show version information";

int versionMain(std::span<const StringRef>) noexcept;
void versionHelp() noexcept;
