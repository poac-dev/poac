#pragma once

// external
#include <spdlog/details/os.h>
#include <spdlog/spdlog.h>

namespace termcolor2::details {

class ColorMode {
public:
  inline void set(spdlog::color_mode mode) {
    switch (mode) {
      case spdlog::color_mode::always:
        should_color_ = true;
        return;
      case spdlog::color_mode::automatic:
        should_color_ = spdlog::details::os::is_color_terminal();
        return;
      case spdlog::color_mode::never:
        should_color_ = false;
        return;
      default:
        __builtin_unreachable();
    }
  }
  [[nodiscard]] inline auto should_color() const -> bool {
    return should_color_;
  }

  static auto instance() -> ColorMode& {
    static ColorMode s_instance;
    return s_instance;
  }

private:
  // default: automatic
  bool should_color_ = spdlog::details::os::is_color_terminal();
};

} // namespace termcolor2::details

namespace termcolor2 {

inline void set_color_mode(spdlog::color_mode cm) {
  details::ColorMode::instance().set(cm);
}

inline auto should_color() -> bool {
  return details::ColorMode::instance().should_color();
}

} // namespace termcolor2
