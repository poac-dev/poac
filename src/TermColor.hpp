#pragma once

#include "Rustify.hpp"

enum class ColorMode {
  always,
  automatic,
  never,
};

void setColorMode(ColorMode cm) noexcept;
void setColorMode(StringRef str) noexcept;
bool shouldColor() noexcept;

String gray(StringRef str, bool force = false) noexcept;
String red(StringRef str, bool force = false) noexcept;
String green(StringRef str, bool force = false) noexcept;
String yellow(StringRef str, bool force = false) noexcept;
String blue(StringRef str, bool force = false) noexcept;
String magenta(StringRef str, bool force = false) noexcept;
String cyan(StringRef str, bool force = false) noexcept;

String bold(StringRef str, bool force = false) noexcept;
