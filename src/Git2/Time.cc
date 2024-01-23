#include "Time.hpp"

#include <array>
#include <ctime>
#include <iomanip>

namespace git2 {

String
Time::toString() const {
  const auto t = static_cast<std::time_t>(time);
  std::tm* tm = std::localtime(&t);
  String buffer(10, '\0');
  if (std::strftime(buffer.data(), buffer.size() + 1, "%Y-%m-%d", tm) == 0) {
    return {};
  }
  return buffer;
}

} // namespace git2
