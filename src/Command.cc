#include "Command.hpp"

#include "Exception.hpp"
#include "Rustify.hpp"

#include <array>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// NOLINTBEGIN

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
    throw PoacError("waitpid() failed");
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
  if (stdoutfd > maxfd) {
    maxfd = stdoutfd;
  }
  if (stderrfd > maxfd) {
    maxfd = stderrfd;
  }

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

    int ret = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
    if (ret == -1) {
      if (stdoutfd != -1) {
        close(stdoutfd);
      }
      if (stderrfd != -1) {
        close(stderrfd);
      }
      throw PoacError("select() failed");
    }

    // Read from stdout if available
    if (!stdoutEOF && FD_ISSET(stdoutfd, &readfds)) {
      char buffer[128];
      ssize_t count = read(stdoutfd, buffer, sizeof(buffer) - 1);
      if (count == -1) {
        if (stdoutfd != -1) {
          close(stdoutfd);
        }
        if (stderrfd != -1) {
          close(stderrfd);
        }
        throw PoacError("read() failed on stdout");
      } else if (count == 0) {
        stdoutEOF = true;
        close(stdoutfd);
      } else {
        buffer[count] = '\0';
        stdoutOutput += buffer;
      }
    }

    // Read from stderr if available
    if (!stderrEOF && FD_ISSET(stderrfd, &readfds)) {
      char buffer[128];
      ssize_t count = read(stderrfd, buffer, sizeof(buffer) - 1);
      if (count == -1) {
        if (stdoutfd != -1) {
          close(stdoutfd);
        }
        if (stderrfd != -1) {
          close(stderrfd);
        }
        throw PoacError("read() failed on stderr");
      } else if (count == 0) {
        stderrEOF = true;
        close(stderrfd);
      } else {
        buffer[count] = '\0';
        stderrOutput += buffer;
      }
    }
  }

  int status{};
  if (waitpid(pid, &status, 0) == -1) {
    throw PoacError("waitpid() failed");
  }

  int exitCode = WEXITSTATUS(status);
  return { .exitCode = exitCode,
           .stdout = stdoutOutput,
           .stderr = stderrOutput };
}

Child
Command::spawn() const {
  int stdoutPipe[2];
  int stderrPipe[2];

  // Set up stdout pipe if needed
  if (stdoutConfig == IOConfig::Piped) {
    if (pipe(stdoutPipe) == -1) {
      throw PoacError("pipe() failed for stdout");
    }
  }
  // Set up stderr pipe if needed
  if (stderrConfig == IOConfig::Piped) {
    if (pipe(stderrPipe) == -1) {
      throw PoacError("pipe() failed for stderr");
    }
  }

  const pid_t pid = fork();
  if (pid == -1) {
    throw PoacError("fork() failed");
  } else if (pid == 0) {
    // Child process

    // Redirect stdout
    if (stdoutConfig == IOConfig::Piped) {
      close(stdoutPipe[0]); // Child doesn't read from stdout pipe
      dup2(stdoutPipe[1], STDOUT_FILENO);
      close(stdoutPipe[1]);
    } else if (stdoutConfig == IOConfig::Null) {
      int nullfd = open("/dev/null", O_WRONLY);
      dup2(nullfd, STDOUT_FILENO);
      close(nullfd);
    }

    // Redirect stderr
    if (stderrConfig == IOConfig::Piped) {
      close(stderrPipe[0]); // Child doesn't read from stderr pipe
      dup2(stderrPipe[1], STDERR_FILENO);
      close(stderrPipe[1]);
    } else if (stderrConfig == IOConfig::Null) {
      int nullfd = open("/dev/null", O_WRONLY);
      dup2(nullfd, STDERR_FILENO);
      close(nullfd);
    }

    std::vector<char*> args;
    args.push_back(const_cast<char*>(command.c_str()));
    for (const std::string& arg : arguments) {
      args.push_back(const_cast<char*>(arg.c_str()));
    }
    args.push_back(nullptr);

    if (!workingDirectory.empty()) {
      if (chdir(workingDirectory.c_str()) == -1) {
        throw PoacError("chdir() failed");
      }
    }

    // Execute the command
    if (execvp(command.c_str(), args.data()) == -1) {
      throw PoacError("execvp() failed");
    }
    unreachable();
  } else {
    // Parent process

    // Close unused pipe ends
    if (stdoutConfig == IOConfig::Piped) {
      close(stdoutPipe[1]); // Parent doesn't write to stdout pipe
    }
    if (stderrConfig == IOConfig::Piped) {
      close(stderrPipe[1]); // Parent doesn't write to stderr pipe
    }

    // Return the Child object with appropriate file descriptors
    return { pid, stdoutConfig == IOConfig::Piped ? stdoutPipe[0] : -1,
             stderrConfig == IOConfig::Piped ? stderrPipe[0] : -1 };
  }
}

// NOLINTEND

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
