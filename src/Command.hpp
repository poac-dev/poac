#pragma once

#include <ostream>
#include <string>
#include <string_view>
#include <vector>

struct CommandOutput {
  std::string output;
  int exitCode;
};

struct Command {
  std::string command;
  std::vector<std::string> arguments;

  explicit Command(const std::string_view cmd) : command(cmd) {}

  Command& addArg(const std::string_view arg) {
    arguments.emplace_back(arg);
    return *this;
  }

  std::string getCmdline() const {
    std::string cmdline = command;
    for (const std::string& arg : arguments) {
      cmdline = cmdline + ' ' + arg;
    }
    return cmdline;
  }

  int execute() const;

  CommandOutput output() const;
};

std::ostream&
operator<<(std::ostream& os, const Command& cmd) {
  os << cmd.command;
  for (const std::string& arg : cmd.arguments) {
    os << " " << arg;
  }
  return os;
}
