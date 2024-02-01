#pragma once

#include "Rustify.hpp"

enum class ColorMode {
  Always,
  Auto,
  Never,
};

void setColorMode(ColorMode mode) noexcept;
void setColorMode(StringRef str) noexcept;
bool shouldColor() noexcept;

String gray(StringRef str) noexcept;
String red(StringRef str) noexcept;
String green(StringRef str) noexcept;
String yellow(StringRef str) noexcept;
String blue(StringRef str) noexcept;
String magenta(StringRef str) noexcept;
String cyan(StringRef str) noexcept;

String bold(StringRef str) noexcept;
