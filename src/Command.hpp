#pragma once

#include <cstdint>
#include <filesystem>
#include <ostream>
#include <string>
#include <string_view>
#include <sys/types.h>
#include <utility>
#include <vector>

struct CommandOutput {
  int exitCode;
  std::string stdout;
  std::string stderr;
};

class Child {
private:
  pid_t pid;
  int stdoutfd;
  int stderrfd;

  Child(pid_t pid, int stdoutfd, int stderrfd) noexcept
      : pid(pid), stdoutfd(stdoutfd), stderrfd(stderrfd) {}

  friend struct Command;

public:
  int wait() const;
  CommandOutput waitWithOutput() const;
};

struct Command {
  enum class IOConfig : uint8_t {
    Null,
    Inherit,
    Piped,
  };

  std::string command;
  std::vector<std::string> arguments;
  std::filesystem::path workingDirectory;
  IOConfig stdoutConfig = IOConfig::Inherit;
  IOConfig stderrConfig = IOConfig::Inherit;

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

  Command& setStdoutConfig(IOConfig config) noexcept {
    stdoutConfig = config;
    return *this;
  }
  Command& setStderrConfig(IOConfig config) noexcept {
    stderrConfig = config;
    return *this;
  }
  Command& setWorkingDirectory(const std::filesystem::path& dir) {
    workingDirectory = dir;
    return *this;
  }

  std::string toString() const;

  Child spawn() const;
  CommandOutput output() const;
};

std::ostream& operator<<(std::ostream& os, const Command& cmd);
