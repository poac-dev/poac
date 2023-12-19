#pragma once

#include "Rustify.hpp"

enum class ColorMode {
  always,
  automatic,
  never,
};

void set_color_mode(ColorMode) noexcept;
bool should_color() noexcept;

String gray(const String&) noexcept;
String red(const String&) noexcept;
String green(const String&) noexcept;
String yellow(const String&) noexcept;
String blue(const String&) noexcept;
String magenta(const String&) noexcept;
String cyan(const String&) noexcept;
String white(const String&) noexcept;

String bold(const String&) noexcept;
