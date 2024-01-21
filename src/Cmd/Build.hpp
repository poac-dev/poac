#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef buildDesc =
    "Compile a local package and all of its dependencies";

void buildHelp() noexcept;
int buildImpl(String& outDir, bool isDebug, bool isParallel);
int buildMain(std::span<const StringRef> args);
