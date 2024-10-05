#pragma once

#include <cstdint>
#include <ostream>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <utility>
#include <vector>

struct CommandOutput {
  std::string output;
  int exitCode;
};

class Child {
private:
  pid_t pid;
  int stdoutfd;

  Child(pid_t pid, int stdoutfd) noexcept : pid(pid), stdoutfd(stdoutfd) {}

  friend struct Command;

public:
  int wait() const;
  CommandOutput waitWithOutput() const;
};

struct Command {
  enum class StdioConfig : std::uint8_t {
    Null,
    Inherit,
    Piped,
  };

  std::string command;
  std::vector<std::string> arguments;
  std::string workingDirectory;
  StdioConfig stdoutConfig = StdioConfig::Inherit;

  explicit Command(std::string_view cmd) : command(cmd) {}
  Command(std::string_view cmd, std::vector<std::string> args)
      : command(cmd), arguments(std::move(args)) {}

  Command& addArg(const std::string_view arg) {
    arguments.emplace_back(arg);
    return *this;
  }

  Command& addArgs(const std::vector<std::string>& args) {
    arguments.insert(arguments.end(), args.begin(), args.end());
    return *this;
  }

  Command& setStdoutConfig(StdioConfig config) noexcept {
    stdoutConfig = config;
    return *this;
  }

  Command& setWorkingDirectory(const std::string_view dir) {
    workingDirectory = dir;
    return *this;
  }

  std::string toString() const;

  Child spawn() const;

  CommandOutput output() const;
};

std::ostream& operator<<(std::ostream& os, const Command& cmd);
