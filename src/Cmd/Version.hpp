#pragma once

#include "../Rustify.hpp"

#define POAC_VERSION "0.6.0" // TODO: Get from poac.toml

static inline constexpr StringRef versionDesc = "Show version information";

int versionMain(Vec<String>) noexcept;
void versionHelp() noexcept;
