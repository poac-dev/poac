#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef fmtDesc = "Format codes using clang-format";

int fmtMain(std::span<const StringRef>);
void fmtHelp() noexcept;
