#include "Command.hpp"

#include "Exception.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"

#include <array>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int
Child::wait() const {

  int status;
  if (waitpid(pid, &status, 0) == -1) {
    close(stdoutfd);
    throw PoacError("waitpid() failed");
  }

  close(stdoutfd);

  const int exitCode = WEXITSTATUS(status);
  return exitCode;
}

CommandOutput
Child::wait_with_output() const {
  constexpr std::size_t bufferSize = 128;
  std::array<char, bufferSize> buffer{};
  std::string output;

  FILE* stream = fdopen(stdoutfd, "r");
  if (stream == nullptr) {
    close(stdoutfd);
    throw PoacError("fdopen() failed");
  }

  while (fgets(buffer.data(), buffer.size(), stream) != nullptr) {
    output += buffer.data();
  }

  fclose(stream);

  int status;
  if (waitpid(pid, &status, 0) == -1) {
    throw PoacError("waitpid() failed");
  }

  const int exitCode = WEXITSTATUS(status);
  return { output, exitCode };
}

Child
Command::spawn() const {
  int pipefd[2];

  if (pipe(pipefd) == -1) {
    throw PoacError("pipe() failed");
  }

  pid_t pid = fork();
  if (pid == -1) {
    throw PoacError("fork() failed");
  } else if (pid == 0) {
    close(pipefd[0]); // child doesn't read

    // redirect stdout to pipe
    dup2(pipefd[1], 1);
    close(pipefd[1]);

    std::vector<char*> args;
    args.push_back(const_cast<std::string&>(command).data());
    for (std::string& arg : const_cast<std::vector<std::string>&>(arguments)) {
      args.push_back(arg.data());
    }
    args.push_back(nullptr);

    if (!working_directory.empty()) {
      if (chdir(working_directory.c_str()) == -1) {
        throw PoacError("chdir() failed");
      }
    }

    if (execvp(command.data(), args.data()) == -1) {
      throw PoacError("execvp() failed");
    }
    unreachable();
  } else {
    close(pipefd[1]); // parent doesn't write

    return Child(pid, pipefd[0]);
  }
}

CommandOutput
Command::output() const {
  return spawn().wait_with_output();
}

std::string
Command::to_string() const {
  std::string res = command;
  for (const std::string& arg : arguments) {
    res += ' ' + arg;
  }
  return res;
}

std::ostream&
operator<<(std::ostream& os, const Command& cmd) {
  return os << cmd.to_string();
}
