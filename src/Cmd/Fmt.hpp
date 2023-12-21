#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef fmtDesc = "Format codes using clang-format";

int fmtMain(Vec<String>);
void fmtHelp() noexcept;
