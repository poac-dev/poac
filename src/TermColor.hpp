#pragma once

#include "Rustify.hpp"

enum class ColorMode {
  always,
  automatic,
  never,
};

void set_color_mode(ColorMode);
bool should_color();

String gray(const String&);
String red(const String&);
String green(const String&);
String yellow(const String&);
String blue(const String&);
String magenta(const String&);
String cyan(const String&);
String white(const String&);

String on_gray(const String&);
String on_red(const String&);
String on_green(const String&);
String on_yellow(const String&);
String on_blue(const String&);
String on_magenta(const String&);
String on_cyan(const String&);
String on_white(const String&);

String bold(const String&);
String underline(const String&);
String blink(const String&);
String reverse(const String&);
String concealed(const String&);
