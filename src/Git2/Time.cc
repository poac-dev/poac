#include "Time.hpp"

#include <ctime>
#include <iomanip>

namespace git2 {

String
Time::toString() const {
  const auto t = static_cast<std::time_t>(time);
  std::tm* tm = std::localtime(&t);
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
  char buffer[11];
  if (std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm) == 0) {
    return {};
  }
  return { buffer };
}

} // namespace git2
