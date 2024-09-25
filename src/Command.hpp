#pragma once

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
  CommandOutput wait_with_output() const;
};

struct Command {
  enum class StdioConfig {
    Null,
    Inherit,
    Piped,
  };

  std::string command;
  std::vector<std::string> arguments;
  std::string working_directory;
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

  Command& setWorkingDirectory(const std::string_view wd) {
    working_directory = wd;
    return *this;
  }

  std::string to_string() const;

  Child spawn() const;

  CommandOutput output() const;
};

std::ostream& operator<<(std::ostream& os, const Command& cmd);
