#include "TermColor.hpp"

#include "Logger.hpp"
#include "Rustify.hpp"

#include <cstdlib>

static bool
isTerm() noexcept {
  return std::getenv("TERM") != nullptr;
}

static ColorMode
getColorMode(const StringRef str) noexcept {
  if (str == "always") {
    return ColorMode::always;
  } else if (str == "auto") {
    return ColorMode::automatic;
  } else if (str == "never") {
    return ColorMode::never;
  } else {
    Logger::warn("unknown color mode `", str, "`; falling back to auto");
    return ColorMode::automatic;
  }
}

struct ColorState {
  // ColorState is a singleton
  ColorState(const ColorState&) = delete;
  ColorState& operator=(const ColorState&) = delete;
  ColorState(ColorState&&) noexcept = delete;
  ColorState& operator=(ColorState&&) noexcept = delete;
  ~ColorState() noexcept = default;

  void set(const ColorMode mode) noexcept {
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
  inline bool shouldColor() const noexcept {
    return should_color_;
  }

  static ColorState& instance() noexcept {
    static ColorState INSTANCE;
    return INSTANCE;
  }

private:
  // default: automatic
  bool should_color_;

  ColorState() noexcept {
    if (const char* color = std::getenv("POAC_TERM_COLOR")) {
      set(getColorMode(color));
    } else {
      should_color_ = isTerm();
    }
  }
};

void
setColorMode(const ColorMode cm) noexcept {
  ColorState::instance().set(cm);
}

void
setColorMode(const StringRef str) noexcept {
  setColorMode(getColorMode(str));
}

bool
shouldColor() noexcept {
  return ColorState::instance().shouldColor();
}

static String
colorize(
    const StringRef str, const StringRef color, const bool force
) noexcept {
  if (force || shouldColor()) {
    return String(color) + String(str) + "\033[0m";
  }
  return String(str);
}

String
gray(const StringRef str, const bool force) noexcept {
  return colorize(str, "\033[30m", force);
}
String
red(const StringRef str, const bool force) noexcept {
  return colorize(str, "\033[31m", force);
}
String
green(const StringRef str, const bool force) noexcept {
  return colorize(str, "\033[32m", force);
}
String
yellow(const StringRef str, const bool force) noexcept {
  return colorize(str, "\033[33m", force);
}
String
blue(const StringRef str, const bool force) noexcept {
  return colorize(str, "\033[34m", force);
}
String
magenta(const StringRef str, const bool force) noexcept {
  return colorize(str, "\033[35m", force);
}
String
cyan(const StringRef str, const bool force) noexcept {
  return colorize(str, "\033[36m", force);
}

String
bold(const StringRef str, const bool force) noexcept {
  return colorize(str, "\033[1m", force);
}
