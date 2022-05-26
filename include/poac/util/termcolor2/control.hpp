#ifndef POAC_UTIL_TERMCOLOR2_CONTROL_HPP_
#define POAC_UTIL_TERMCOLOR2_CONTROL_HPP_

#include <spdlog/details/os.h>
#include <spdlog/spdlog.h>

namespace termcolor2::details {

class Control {
public:
  inline void
  set_level(spdlog::color_mode mode) {
    switch (mode) {
      case spdlog::color_mode::always:
        should_do_colors_ = true;
        return;
      case spdlog::color_mode::automatic:
        should_do_colors_ = spdlog::details::os::is_color_terminal();
        return;
      case spdlog::color_mode::never:
        should_do_colors_ = false;
        return;
      default:
        __builtin_unreachable();
    }
  }
  inline bool
  should_color() {
    return should_do_colors_;
  }

  static Control&
  instance() {
    static Control s_instance;
    return s_instance;
  }

private:
  bool should_do_colors_ = false;
};

} // namespace termcolor2::details

namespace termcolor2 {

inline void
set_color_mode(spdlog::color_mode cm) {
  details::Control::instance().set_level(cm);
}

inline bool
should_color() {
  return details::Control::instance().should_color();
}

} // namespace termcolor2

#endif // POAC_UTIL_TERMCOLOR2_CONTROL_HPP_
