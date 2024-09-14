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

CommandOutput
Command::output() const {
  constexpr std::size_t bufferSize = 128;
  std::array<char, bufferSize> buffer{};
  std::string output;
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
    if (execvp(command.data(), args.data()) == -1) {
      throw PoacError("execvp() failed");
    }
    std::terminate(); // unreachable
  } else {
    close(pipefd[1]); // parent doesn't write

    int status;
    if (waitpid(pid, &status, 0) == -1) {
      throw PoacError("waitpid() failed");
    }

    FILE* stream = fdopen(pipefd[0], "r");
    if (stream == nullptr) {
      close(pipefd[0]);
      throw PoacError("fdopen() failed");
    }

    while (fgets(buffer.data(), buffer.size(), stream) != nullptr) {
      output += buffer.data();
    }

    fclose(stream);

    const int exitCode = WEXITSTATUS(status);
    return { output, exitCode };
  }
}
