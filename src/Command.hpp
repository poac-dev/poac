#pragma once

#include <cstdlib>
#include <ostream>
#include <string>
#include <vector>

struct Command {
  std::string command;
  std::vector<std::string> arguments;

  Command(const std::string_view cmd) : command(cmd) {}

  Command& addArg(const std::string_view arg) {
    arguments.emplace_back(arg);
    return *this;
  }

  int execute() const {
    std::string cmdline = command;
    for (const std::string& arg : arguments) {
      cmdline += arg;
    }
    int status = std::system(cmdline.c_str());
    return WEXITSTATUS(status);
  }
};

std::ostream&
operator<<(std::ostream& os, const Command& cmd) {
  os << cmd.command;
  for (const std::string& arg : cmd.arguments) {
    os << " " << arg;
  }
  return os;
}
