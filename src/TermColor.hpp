#pragma once

#include "Rustify.hpp"

enum class ColorMode {
  always,
  automatic,
  never,
};

void set_color_mode(ColorMode) noexcept;
bool should_color() noexcept;

String gray(StringRef) noexcept;
String red(StringRef) noexcept;
String green(StringRef) noexcept;
String yellow(StringRef) noexcept;
String blue(StringRef) noexcept;
String magenta(StringRef) noexcept;
String cyan(StringRef) noexcept;

String bold(StringRef) noexcept;
