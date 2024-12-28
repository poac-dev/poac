#include "Command.hpp"

#include "Exception.hpp"
#include "Rustify.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

constexpr std::size_t BUFFER_SIZE = 128;

int
Child::wait() const {
  int status{};
  if (waitpid(pid, &status, 0) == -1) {
    if (stdoutfd != -1) {
      close(stdoutfd);
    }
    if (stderrfd != -1) {
      close(stderrfd);
    }
    throw CabinError("waitpid() failed");
  }

  if (stdoutfd != -1) {
    close(stdoutfd);
  }
  if (stderrfd != -1) {
    close(stderrfd);
  }

  const int exitCode = WEXITSTATUS(status);
  return exitCode;
}

CommandOutput
Child::waitWithOutput() const {
  std::string stdoutOutput;
  std::string stderrOutput;

  int maxfd = -1;
  fd_set readfds;

  // Determine the maximum file descriptor
  maxfd = std::max(maxfd, stdoutfd);
  maxfd = std::max(maxfd, stderrfd);

  bool stdoutEOF = (stdoutfd == -1);
  bool stderrEOF = (stderrfd == -1);

  while (!stdoutEOF || !stderrEOF) {
    FD_ZERO(&readfds);
    if (!stdoutEOF) {
      FD_SET(stdoutfd, &readfds);
    }
    if (!stderrEOF) {
      FD_SET(stderrfd, &readfds);
    }

    const int ret = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
    if (ret == -1) {
      if (stdoutfd != -1) {
        close(stdoutfd);
      }
      if (stderrfd != -1) {
        close(stderrfd);
      }
      throw CabinError("select() failed");
    }

    // Read from stdout if available
    if (!stdoutEOF && FD_ISSET(stdoutfd, &readfds)) {
      std::array<char, BUFFER_SIZE> buffer{};
      const ssize_t count = read(stdoutfd, buffer.data(), buffer.size());
      if (count == -1) {
        if (stdoutfd != -1) {
          close(stdoutfd);
        }
        if (stderrfd != -1) {
          close(stderrfd);
        }
        throw CabinError("read() failed on stdout");
      } else if (count == 0) {
        stdoutEOF = true;
        close(stdoutfd);
      } else {
        stdoutOutput.append(buffer.data(), static_cast<std::size_t>(count));
      }
    }

    // Read from stderr if available
    if (!stderrEOF && FD_ISSET(stderrfd, &readfds)) {
      std::array<char, BUFFER_SIZE> buffer{};
      const ssize_t count = read(stderrfd, buffer.data(), buffer.size());
      if (count == -1) {
        if (stdoutfd != -1) {
          close(stdoutfd);
        }
        if (stderrfd != -1) {
          close(stderrfd);
        }
        throw CabinError("read() failed on stderr");
      } else if (count == 0) {
        stderrEOF = true;
        close(stderrfd);
      } else {
        stderrOutput.append(buffer.data(), static_cast<std::size_t>(count));
      }
    }
  }

  int status{};
  if (waitpid(pid, &status, 0) == -1) {
    throw CabinError("waitpid() failed");
  }

  const int exitCode = WEXITSTATUS(status);
  return { .exitCode = exitCode,
           .stdout = stdoutOutput,
           .stderr = stderrOutput };
}

Child
Command::spawn() const {
  std::array<int, 2> stdoutPipe{};
  std::array<int, 2> stderrPipe{};

  // Set up stdout pipe if needed
  if (stdoutConfig == IOConfig::Piped) {
    if (pipe(stdoutPipe.data()) == -1) {
      throw CabinError("pipe() failed for stdout");
    }
  }
  // Set up stderr pipe if needed
  if (stderrConfig == IOConfig::Piped) {
    if (pipe(stderrPipe.data()) == -1) {
      throw CabinError("pipe() failed for stderr");
    }
  }

  const pid_t pid = fork();
  if (pid == -1) {
    throw CabinError("fork() failed");
  } else if (pid == 0) {
    // Child process

    // Redirect stdout
    if (stdoutConfig == IOConfig::Piped) {
      close(stdoutPipe[0]);  // Child doesn't read from stdout pipe
      dup2(stdoutPipe[1], STDOUT_FILENO);
      close(stdoutPipe[1]);
    } else if (stdoutConfig == IOConfig::Null) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
      const int nullfd = open("/dev/null", O_WRONLY);
      dup2(nullfd, STDOUT_FILENO);
      close(nullfd);
    }

    // Redirect stderr
    if (stderrConfig == IOConfig::Piped) {
      close(stderrPipe[0]);  // Child doesn't read from stderr pipe
      dup2(stderrPipe[1], STDERR_FILENO);
      close(stderrPipe[1]);
    } else if (stderrConfig == IOConfig::Null) {
      // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
      const int nullfd = open("/dev/null", O_WRONLY);
      dup2(nullfd, STDERR_FILENO);
      close(nullfd);
    }

    // Prepare arguments
    std::vector<std::vector<char>> argBuffers;
    std::vector<char*> args;

    // Add command
    argBuffers.emplace_back(command.begin(), command.end());
    argBuffers.back().push_back('\0');
    args.push_back(argBuffers.back().data());

    // Add arguments
    for (const std::string& arg : arguments) {
      argBuffers.emplace_back(arg.begin(), arg.end());
      argBuffers.back().push_back('\0');
      args.push_back(argBuffers.back().data());
    }
    args.push_back(nullptr);

    if (!workingDirectory.empty()) {
      if (chdir(workingDirectory.c_str()) == -1) {
        perror("chdir() failed");
        _exit(1);
      }
    }

    // Execute the command
    if (execvp(command.c_str(), args.data()) == -1) {
      perror("execvp() failed");
      _exit(1);
    }
    unreachable();
  } else {
    // Parent process

    // Close unused pipe ends
    if (stdoutConfig == IOConfig::Piped) {
      close(stdoutPipe[1]);  // Parent doesn't write to stdout pipe
    }
    if (stderrConfig == IOConfig::Piped) {
      close(stderrPipe[1]);  // Parent doesn't write to stderr pipe
    }

    // Return the Child object with appropriate file descriptors
    return { pid, stdoutConfig == IOConfig::Piped ? stdoutPipe[0] : -1,
             stderrConfig == IOConfig::Piped ? stderrPipe[0] : -1 };
  }
}

CommandOutput
Command::output() const {
  Command cmd = *this;
  cmd.setStdoutConfig(IOConfig::Piped);
  cmd.setStderrConfig(IOConfig::Piped);
  return cmd.spawn().waitWithOutput();
}

std::string
Command::toString() const {
  std::string res = command;
  for (const std::string& arg : arguments) {
    res += ' ' + arg;
  }
  return res;
}

std::ostream&
operator<<(std::ostream& os, const Command& cmd) {
  return os << cmd.toString();
}
