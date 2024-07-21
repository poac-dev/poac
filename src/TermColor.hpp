#pragma once

#include "Rustify.hpp"

#include <string>

enum class ColorMode {
  Always,
  Auto,
  Never,
};

void setColorMode(ColorMode mode) noexcept;
void setColorMode(StringRef str) noexcept;
bool shouldColor() noexcept;

std::string gray(StringRef str) noexcept;
std::string red(StringRef str) noexcept;
std::string green(StringRef str) noexcept;
std::string yellow(StringRef str) noexcept;
std::string blue(StringRef str) noexcept;
std::string magenta(StringRef str) noexcept;
std::string cyan(StringRef str) noexcept;

std::string bold(StringRef str) noexcept;
