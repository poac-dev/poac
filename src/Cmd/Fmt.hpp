#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef fmtDesc = "Format codes using clang-format";

int fmtMain(std::span<const StringRef> args);
void fmtHelp() noexcept;
