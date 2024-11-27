#include "Command.hpp"

#include "Exception.hpp"
#include "Rustify.hpp"

#include <array>
#include <string>
#include <vector>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <fcntl.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif

constexpr std::size_t BUFFER_SIZE = 128;

#ifdef _WIN32
namespace {
std::string
GetLastErrorAsString() {
  DWORD error = GetLastError();
  char buffer[256];
  FormatMessageA(
      FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, error,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, sizeof(buffer), NULL
  );
  return std::string(buffer);
}

void
CloseHandleSafe(HANDLE& handle) {
  if (handle != INVALID_HANDLE_VALUE && handle != NULL) {
    CloseHandle(handle);
    handle = INVALID_HANDLE_VALUE;
  }
}
}  // namespace
#endif

int
Child::wait() const {
#ifdef _WIN32
  DWORD exitCode = 0;
  WaitForSingleObject(process, INFINITE);
  GetExitCodeProcess(process, &exitCode);

  CloseHandleSafe(const_cast<HANDLE&>(process));
  CloseHandleSafe(const_cast<HANDLE&>(stdoutfd));
  CloseHandleSafe(const_cast<HANDLE&>(stderrfd));

  return static_cast<int>(exitCode);
#else
  int status{};
  if (waitpid(process, &status, 0) == -1) {
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
#endif
}

CommandOutput
Child::waitWithOutput() const {
#ifdef _WIN32
  std::string stdoutOutput;
  std::string stderrOutput;
  DWORD bytesRead;
  std::array<char, BUFFER_SIZE> buffer{};

  // Read from stdout
  if (stdoutfd != INVALID_HANDLE_VALUE) {
    while (ReadFile(
               (HANDLE)stdoutfd, buffer.data(), buffer.size(), &bytesRead, NULL
           )
           && bytesRead > 0) {
      stdoutOutput.append(buffer.data(), bytesRead);
    }
  }

  // read from stderr
  if (stderrfd != INVALID_HANDLE_VALUE) {
    while (ReadFile(
               (HANDLE)stderrfd, buffer.data(), buffer.size(), &bytesRead, NULL
           )
           && bytesRead > 0) {
      stderrOutput.append(buffer.data(), bytesRead);
    }
  }

  DWORD exitCode = EXIT_SUCCESS;
  WaitForSingleObject(process, INFINITE);
  GetExitCodeProcess(process, &exitCode);

  CloseHandleSafe(const_cast<HANDLE&>(process));
  CloseHandleSafe(const_cast<HANDLE&>(stdoutfd));
  CloseHandleSafe(const_cast<HANDLE&>(stderrfd));

  return { .exitCode = static_cast<int>(exitCode),
           .stdout_str = stdoutOutput,
           .stderr_str = stderrOutput };
#else
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
      throw PoacError("select() failed");
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
        throw PoacError("read() failed on stdout");
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
        throw PoacError("read() failed on stderr");
      } else if (count == 0) {
        stderrEOF = true;
        close(stderrfd);
      } else {
        stderrOutput.append(buffer.data(), static_cast<std::size_t>(count));
      }
    }
  }

  int status{};
  if (waitpid(process, &status, 0) == -1) {
    throw PoacError("waitpid() failed");
  }

  const int exitCode = WEXITSTATUS(status);
  return { .exitCode = exitCode,
           .stdout_str = stdoutOutput,
           .stderr_str = stderrOutput };
#endif
}

Child
Command::spawn() const {
#ifdef _WIN32
  SECURITY_ATTRIBUTES saAttr = {};
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;

  HANDLE stdoutRead = INVALID_HANDLE_VALUE;
  HANDLE stdoutWrite = INVALID_HANDLE_VALUE;
  HANDLE stderrRead = INVALID_HANDLE_VALUE;
  HANDLE stderrWrite = INVALID_HANDLE_VALUE;

  if (stdoutConfig == IOConfig::Piped) {
    if (!CreatePipe(&stdoutRead, &stdoutWrite, &saAttr, 0)) {
      throw PoacError("CreatePipe failed for stdout: ", GetLastErrorAsString());
    }
    SetHandleInformation(stdoutRead, HANDLE_FLAG_INHERIT, 0);
  }

  if (stderrConfig == IOConfig::Piped) {
    if (!CreatePipe(&stderrRead, &stderrWrite, &saAttr, 0)) {
      CloseHandleSafe(stdoutRead);
      CloseHandleSafe(stdoutWrite);
      throw PoacError("CreatePipe failed for stderr: ", GetLastErrorAsString());
    }
    SetHandleInformation(stderrRead, HANDLE_FLAG_INHERIT, 0);
  }

  // Prepare startup information
  STARTUPINFOA si = {};
  si.cb = sizeof(si);
  si.dwFlags |= STARTF_USESTDHANDLES;
  si.hStdOutput = stdoutConfig == IOConfig::Piped ? stdoutWrite
                  : stdoutConfig == IOConfig::Null
                      ? NULL
                      : GetStdHandle(STD_OUTPUT_HANDLE);
  si.hStdError = stderrConfig == IOConfig::Piped ? stderrWrite
                 : stderrConfig == IOConfig::Null
                     ? NULL
                     : GetStdHandle(STD_ERROR_HANDLE);
  si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);

  // Prepare command line
  std::string cmdLine = command;
  for (const auto& arg : arguments) {
    cmdLine += " " + arg;
  }

  PROCESS_INFORMATION pi = {};

  // Create process
  BOOL success = CreateProcessA(
      NULL,
      cmdLine.data(),
      NULL,
      NULL,
      TRUE,
      0,
      NULL,
      workingDirectory.empty() ? NULL : workingDirectory.string().c_str(),
      &si,
      &pi
  );

  // Clean up unnecessary handles
  if (stdoutWrite != INVALID_HANDLE_VALUE)
    CloseHandle(stdoutWrite);
  if (stderrWrite != INVALID_HANDLE_VALUE)
    CloseHandle(stderrWrite);

  if (!success) {
    CloseHandleSafe(stdoutRead);
    CloseHandleSafe(stderrRead);
    throw PoacError("CreateProcess failed: ", GetLastErrorAsString());
  }

  CloseHandle(pi.hThread);  // Close thread handle

  return { pi.hProcess,
           stdoutConfig == IOConfig::Piped ? stdoutRead : INVALID_HANDLE_VALUE,
           stderrConfig == IOConfig::Piped ? stderrRead
                                           : INVALID_HANDLE_VALUE };
#else
  std::array<int, 2> stdoutPipe{};
  std::array<int, 2> stderrPipe{};

  // Set up stdout pipe if needed
  if (stdoutConfig == IOConfig::Piped) {
    if (pipe(stdoutPipe.data()) == -1) {
      throw PoacError("pipe() failed for stdout");
    }
  }
  // Set up stderr pipe if needed
  if (stderrConfig == IOConfig::Piped) {
    if (pipe(stderrPipe.data()) == -1) {
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
#endif
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
