#include "Time.hpp"

#include <cstddef>
#include <ctime>
#include <iomanip>
#include <string>

namespace git2 {

std::string
Time::toString() const {
  const auto time = static_cast<std::time_t>(mTime);
  std::tm* time2 = std::localtime(&time);

  constexpr size_t dateLen = 10; // YYYY-MM-DD
  std::string buffer(dateLen, '\0');
  if (std::strftime(
          buffer.data(), dateLen + 1, // null-terminator
          "%Y-%m-%d", time2
      )
      == 0) {
    return {};
  }
  return buffer;
}

} // namespace git2
