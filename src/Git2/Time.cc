#include "Time.hpp"

#include <ctime>
#include <iomanip>

namespace git2 {

String
Time::toString() const {
  const auto time2 = static_cast<std::time_t>(time);
  std::tm* time3 = std::localtime(&time2);

  constexpr usize DATE_LEN = 10; // YYYY-MM-DD
  String buffer(DATE_LEN, '\0');
  if (std::strftime(
          buffer.data(), DATE_LEN + 1, // null-terminator
          "%Y-%m-%d", time3
      )
      == 0) {
    return {};
  }
  return buffer;
}

} // namespace git2
