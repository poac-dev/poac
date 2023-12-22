#pragma once

#include "./Rustify.hpp"

enum class ColorMode {
  always,
  automatic,
  never,
};

void setColorMode(ColorMode) noexcept;
bool shouldColor() noexcept;

String gray(StringRef) noexcept;
String red(StringRef) noexcept;
String green(StringRef) noexcept;
String yellow(StringRef) noexcept;
String blue(StringRef) noexcept;
String magenta(StringRef) noexcept;
String cyan(StringRef) noexcept;

String bold(StringRef) noexcept;
