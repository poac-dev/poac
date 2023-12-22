#pragma once

#include "../Rustify.hpp"

#include <span>

#define POAC_VERSION "0.6.0"

static inline constexpr StringRef versionDesc = "Show version information";

int versionMain(std::span<const StringRef>) noexcept;
void versionHelp() noexcept;
