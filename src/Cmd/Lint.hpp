#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef lintDesc = "Lint codes using cpplint";

int lintMain(std::span<const StringRef>);
void lintHelp() noexcept;
