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
  int mExitCode;
  std::string mStdout;
  std::string mStderr;
};

class Child {
private:
  pid_t mPid;
  int mStdoutfd;
  int mStderrfd;

  Child(pid_t pid, int stdoutfd, int stderrfd) noexcept
      : mPid(pid), mStdoutfd(stdoutfd), mStderrfd(stderrfd) {}

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

  std::string mCommand;
  std::vector<std::string> mArguments;
  std::filesystem::path mWorkingDirectory;
  IOConfig mStdoutConfig = IOConfig::Inherit;
  IOConfig mStderrConfig = IOConfig::Inherit;

  explicit Command(std::string_view cmd) : mCommand(cmd) {}
  Command(std::string_view cmd, std::vector<std::string> args)
      : mCommand(cmd), mArguments(std::move(args)) {}

  Command& addArg(const std::string_view arg) {
    mArguments.emplace_back(arg);
    return *this;
  }
  Command& addArgs(const std::vector<std::string>& args) {
    mArguments.insert(mArguments.end(), args.begin(), args.end());
    return *this;
  }

  Command& setStdoutConfig(IOConfig config) noexcept {
    mStdoutConfig = config;
    return *this;
  }
  Command& setStderrConfig(IOConfig config) noexcept {
    mStderrConfig = config;
    return *this;
  }
  Command& setWorkingDirectory(const std::filesystem::path& dir) {
    mWorkingDirectory = dir;
    return *this;
  }

  std::string toString() const;

  Child spawn() const;
  CommandOutput output() const;
};

std::ostream& operator<<(std::ostream& os, const Command& cmd);
