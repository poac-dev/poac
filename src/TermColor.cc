#include "TermColor.hpp"

static bool isTerm() noexcept {
  return std::getenv("TERM") != nullptr;
}

class ColorState {
public:
  inline void set(ColorMode mode) noexcept {
    switch (mode) {
      case ColorMode::always:
        should_color_ = true;
        return;
      case ColorMode::automatic:
        should_color_ = isTerm();
        return;
      case ColorMode::never:
        should_color_ = false;
        return;
    }
  }
  inline bool should_color() const noexcept {
    return should_color_;
  }

  static ColorState& instance() noexcept {
    static ColorState instance;
    return instance;
  }

private:
  // default: automatic
  bool should_color_ = isTerm();
};

void set_color_mode(ColorMode cm) noexcept {
  ColorState::instance().set(cm);
}

bool should_color() noexcept {
  return ColorState::instance().should_color();
}

static String colorize(const String& str, const String& color) noexcept {
  if (!should_color()) {
    return str;
  }
  return color + str + "\033[0m";
}

String gray(const String& str) noexcept {
  return colorize(str, "\033[30m");
}
String red(const String& str) noexcept {
  return colorize(str, "\033[31m");
}
String green(const String& str) noexcept {
  return colorize(str, "\033[32m");
}
String yellow(const String& str) noexcept {
  return colorize(str, "\033[33m");
}
String blue(const String& str) noexcept {
  return colorize(str, "\033[34m");
}
String magenta(const String& str) noexcept {
  return colorize(str, "\033[35m");
}
String cyan(const String& str) noexcept {
  return colorize(str, "\033[36m");
}
String white(const String& str) noexcept {
  return colorize(str, "\033[37m");
}

String on_gray(const String& str) noexcept {
  return colorize(str, "\033[40m");
}
String on_red(const String& str) noexcept {
  return colorize(str, "\033[41m");
}
String on_green(const String& str) noexcept {
  return colorize(str, "\033[42m");
}
String on_yellow(const String& str) noexcept {
  return colorize(str, "\033[43m");
}
String on_blue(const String& str) noexcept {
  return colorize(str, "\033[44m");
}
String on_magenta(const String& str) noexcept {
  return colorize(str, "\033[45m");
}
String on_cyan(const String& str) noexcept {
  return colorize(str, "\033[46m");
}
String on_white(const String& str) noexcept {
  return colorize(str, "\033[47m");
}

String bold(const String& str) noexcept {
  return colorize(str, "\033[1m");
}
String underline(const String& str) noexcept {
  return colorize(str, "\033[4m");
}
String blink(const String& str) noexcept {
  return colorize(str, "\033[5m");
}
String reverse(const String& str) noexcept {
  return colorize(str, "\033[7m");
}
String concealed(const String& str) noexcept {
  return colorize(str, "\033[8m");
}
