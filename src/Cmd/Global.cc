#include "Global.hpp"

#include "../Algos.hpp"
#include "../Rustify.hpp"
#include "../TermColor.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>

void printHeader(const StringRef header) noexcept {
  std::cout << bold(green(header)) << '\n';
}

void printUsage(const StringRef cmd, const StringRef usage) noexcept {
  std::cout << bold(green("Usage: ")) << bold(cyan("poac "));
  if (!cmd.empty()) {
    std::cout << bold(cyan(cmd)) << ' ';
  }
  std::cout << cyan(usage) << '\n';
}

void printOption(
    const StringRef lng,
    const StringRef shrt,
    const StringRef desc,
    const StringRef placeholder
) noexcept {
  String option;
  if (!shrt.empty()) {
    option += bold(cyan(shrt));
    option += ", ";
  } else {
    // This coloring is for the alignment with std::setw later.
    option += bold(cyan("    "));
  }
  option += bold(cyan(lng));
  option += ' ';
  option += cyan(placeholder);

  if (shouldColor()) {
    std::cout << "  " << std::left << std::setw(69) << option << desc << '\n';
  } else {
    std::cout << "  " << std::left << std::setw(26) << option << desc << '\n';
  }
}

void printCommand(const StringRef name, const StringRef desc) noexcept {
  if (shouldColor()) {
    std::cout << "  " << std::left << std::setw(27) << bold(cyan(name)) << desc
              << '\n';
  } else {
    std::cout << "  " << std::left << std::setw(10) << name << desc << '\n';
  }
}

void printGlobalOpts() noexcept {
  for (const auto& [shrt, lng, placeholder, desc] : GLOBAL_OPT_HELPS) {
    printOption(lng, shrt, desc, placeholder);
  }
}

bool commandExists(const StringRef cmd) noexcept {
  String checkCmd = "command -v ";
  checkCmd += cmd;
  checkCmd += " >/dev/null 2>&1";
  return runCmd(checkCmd) == EXIT_SUCCESS;
}
