module;

// std
#include <memory>
#include <string>
#include <string_view>
#include <utility>

// external
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

export module poac.util.log;

import poac.util.format;

import termcolor2;
import termcolor2.literals_extra;
import termcolor2.to_color_extra;

namespace poac {

//
// String literals
//
using namespace termcolor2::color_literals; // NOLINT(build/namespaces)

} // namespace poac

export namespace poac::log {

//
// Logs
//

// Printed when `--verbose`
template <typename T>
inline void debug(T&& msg) {
  spdlog::debug("[poac] {}", std::forward<T>(msg));
}
template <typename... Args>
inline void debug(fmt::format_string<Args...> fmt, Args&&... args) {
  debug(format(fmt, std::forward<Args>(args)...));
}

// Printed when `no option` & `--verbose`
template <typename T>
inline void status(std::string_view header, T&& msg) {
  if (termcolor2::should_color()) {
    spdlog::info(
        "{:>27} {}", termcolor2::to_bold_green(header), std::forward<T>(msg)
    );
  } else {
    spdlog::info("{:>12} {}", header, std::forward<T>(msg));
  }
}
template <typename... Args>
inline void status(
    std::string_view header, fmt::format_string<Args...> fmt, Args&&... args
) {
  status(header, format(fmt, std::forward<Args>(args)...));
}

template <typename T>
inline void warn(T&& msg) {
  spdlog::warn("{} {}", "Warning:"_bold_yellow, std::forward<T>(msg));
}
template <typename... Args>
inline void warn(fmt::format_string<Args...> fmt, Args&&... args) {
  warn(format(fmt, std::forward<Args>(args)...));
}

template <typename T>
inline void error(const std::shared_ptr<spdlog::logger>& logger, T&& msg) {
  logger->error("{} {}", "Error:"_bold_red, std::forward<T>(msg));
}
template <typename... Args>
inline void error(
    const std::shared_ptr<spdlog::logger>& logger,
    fmt::format_string<Args...> fmt, Args&&... args
) {
  error(logger, format(fmt, std::forward<Args>(args)...));
}

} // namespace poac::log
