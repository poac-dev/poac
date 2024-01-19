#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef tidyDesc = "Run clang-tidy";

int tidyMain(std::span<const StringRef>);
void tidyHelp() noexcept;
