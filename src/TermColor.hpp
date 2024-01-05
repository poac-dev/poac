#pragma once

#include "Rustify.hpp"

enum class ColorMode {
  always,
  automatic,
  never,
};

void setColorMode(const ColorMode) noexcept;
void setColorMode(const StringRef) noexcept;
bool shouldColor() noexcept;

String gray(const StringRef) noexcept;
String red(const StringRef) noexcept;
String green(const StringRef) noexcept;
String yellow(const StringRef) noexcept;
String blue(const StringRef) noexcept;
String magenta(const StringRef) noexcept;
String cyan(const StringRef) noexcept;

String bold(const StringRef) noexcept;
