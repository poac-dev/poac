#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef cleanDesc = "Remove the built directory";

int cleanMain(std::span<const StringRef> args) noexcept;
void cleanHelp() noexcept;
