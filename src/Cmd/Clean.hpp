#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
inline constexpr StringRef cleanDesc = "Remove the built directory";

void cleanHelp() noexcept;
int cleanMain(std::span<const StringRef> args) noexcept;
