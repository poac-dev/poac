#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
inline constexpr StringRef fmtDesc = "Format codes using clang-format";

void fmtHelp() noexcept;
int fmtMain(std::span<const StringRef> args);
