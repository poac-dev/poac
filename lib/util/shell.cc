// std
#include <array>

// internal
#include "poac/util/shell.hpp"

namespace poac::util::shell {

Option<String>
Cmd::exec() const {
  std::array<char, 128> buffer{};
  String result;

#ifdef _WIN32
  if (FILE* pipe = _popen(cmd.c_str(), "r")) {
#else
  if (FILE* pipe = popen(cmd.c_str(), "r")) {
#endif
    while (std::fgets(buffer.data(), 128, pipe) != nullptr)
      result += buffer.data();
#ifdef _WIN32
    if (const i32 code = _pclose(pipe); code != 0) {
#else
    if (const i32 code = pclose(pipe); code != 0) {
#endif
      std::cout << result;
      // TODO(ken-matsui): When errored and piped errors to stdout,
      //  I want to return result stored by them.
      return None;
    }
  } else {
    return None;
  }
  return result;
}

} // namespace poac::util::shell
