#include "Command.hpp"

#include "Exception.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <array>
#include <stdio.h>
#include <stdlib.h>

int
Command::execute() const {
  const std::string cmdline = getCmdline();
  logger::debug("Running `", cmdline, '`');
  const int status = system(cmdline.c_str());
  return WEXITSTATUS(status);
}

CommandOutput
Command::output() const {
  const std::string cmdline = getCmdline();
  constexpr usize bufferSize = 128;
  std::array<char, bufferSize> buffer{};
  std::string output;

  logger::debug("Running `", cmdline, '`');
  FILE* pipe = popen(cmdline.c_str(), "r");
  if (!pipe) {
    throw PoacError("popen() failed!");
  }

  while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
    output += buffer.data();
  }

  const int status = pclose(pipe);
  if (status == -1) {
    throw PoacError("pclose() failed!");
  }
  const int exitCode = WEXITSTATUS(status);
  return { output, exitCode };
}
