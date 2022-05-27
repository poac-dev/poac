#ifndef POAC_UTIL_TERMCOLOR2_COLOR_MODE_HPP_
#define POAC_UTIL_TERMCOLOR2_COLOR_MODE_HPP_

// external
#include <spdlog/details/os.h>
#include <spdlog/spdlog.h>

namespace termcolor2::details {

class ColorMode {
public:
  inline void
  set(spdlog::color_mode mode) {
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
  inline bool
  should_color() const {
    return should_color_;
  }

  static ColorMode&
  instance() {
    static ColorMode s_instance;
    return s_instance;
  }

private:
  // default: automatic
  bool should_color_ = spdlog::details::os::is_color_terminal();
};

} // namespace termcolor2::details

namespace termcolor2 {

inline void
set_color_mode(spdlog::color_mode cm) {
  details::ColorMode::instance().set(cm);
}

inline bool
should_color() {
  return details::ColorMode::instance().should_color();
}

} // namespace termcolor2

#endif // POAC_UTIL_TERMCOLOR2_COLOR_MODE_HPP_
