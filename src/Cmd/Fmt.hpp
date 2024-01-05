#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef fmtDesc = "Format codes using clang-format";

int fmtMain(const std::span<const StringRef>);
void fmtHelp() noexcept;
