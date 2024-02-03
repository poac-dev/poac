#include "Time.hpp"

#include <ctime>
#include <iomanip>

namespace git2 {

String
Time::toString() const {
  const auto time2 = static_cast<std::time_t>(time);
  std::tm* time3 = std::localtime(&time2);

  constexpr usize dateLen = 10; // YYYY-MM-DD
  String buffer(dateLen, '\0');
  if (std::strftime(
          buffer.data(), dateLen + 1, // null-terminator
          "%Y-%m-%d", time3
      )
      == 0) {
    return {};
  }
  return buffer;
}

} // namespace git2
