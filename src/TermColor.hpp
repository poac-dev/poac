#pragma once

#include "Rustify.hpp"

enum class ColorMode {
  always,
  automatic,
  never,
};

void set_color_mode(ColorMode cm);
bool should_color();

String gray(const String& str);
String red(const String& str);
String green(const String& str);
String yellow(const String& str);
String blue(const String& str);
String magenta(const String& str);
String cyan(const String& str);
String white(const String& str);

String on_gray(const String& str);
String on_red(const String& str);
String on_green(const String& str);
String on_yellow(const String& str);
String on_blue(const String& str);
String on_magenta(const String& str);
String on_cyan(const String& str);
String on_white(const String& str);

String bold(const String& str);
String underline(const String& str);
String blink(const String& str);
String reverse(const String& str);
String concealed(const String& str);
