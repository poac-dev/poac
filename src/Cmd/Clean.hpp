#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef cleanDesc = "Remove the built directory";

int cleanMain(std::span<const StringRef>) noexcept;
void cleanHelp() noexcept;
