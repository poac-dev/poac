#include "Command.hpp"

#include "Exception.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <array>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int
Command::execute() const {
  const std::string cmdline = getCmdline();

  int fd[2];

  if (pipe(fd)) {
    throw PoacError("pipe() failed");
  }

  pid_t pid = fork();

  if (pid == -1) {
    close(fd[0]);
    close(fd[1]);
    throw PoacError("fork() failed");
  } else if (pid == 0) {
    std::vector<char*> args;
    args.push_back(const_cast<std::string&>(command).data());
    for (std::string& arg : const_cast<std::vector<std::string>&>(arguments)) {
      args.push_back(arg.data());
    }
    args.push_back(nullptr);
    if (execvp(command.data(), args.data()) == -1) {
      close(fd[1]);
      throw PoacError("execvp() failed");
    }
    close(fd[1]);
    exit(EXIT_SUCCESS);
  } else {
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      close(fd[0]);
      throw PoacError("waitpid() failed");
    }
    close(fd[0]);
    return WEXITSTATUS(status);
  }
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
