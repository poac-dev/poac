#include "Time.hpp"

#include <ctime>
#include <iomanip>

namespace git2 {

String
Time::toString() const {
  const auto time2 = static_cast<std::time_t>(time);
  std::tm* time3 = std::localtime(&time2);
  String buffer(10, '\0');
  if (std::strftime(buffer.data(), buffer.size() + 1, "%Y-%m-%d", time3) == 0) {
    return {};
  }
  return buffer;
}

} // namespace git2
