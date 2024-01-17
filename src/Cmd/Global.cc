#include "Global.hpp"

#include "../Algos.hpp"
#include "../Rustify.hpp"
#include "../TermColor.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>

void
printHeader(const StringRef header) noexcept {
  std::cout << bold(green(header)) << '\n';
}

void
printUsage(const StringRef cmd, const StringRef usage) noexcept {
  std::cout << bold(green("Usage: ")) << bold(cyan("poac "));
  if (!cmd.empty()) {
    std::cout << bold(cyan(cmd)) << ' ';
  }
  std::cout << cyan(usage) << '\n';
}

void
printOption(
    const StringRef lng, const StringRef shrt, const StringRef desc,
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

  std::cout << "  " << std::left;
  if (shouldColor()) {
    std::cout << std::setw(69);
  } else {
    std::cout << std::setw(26);
  }
  std::cout << option << desc << '\n';
}

void
printCommand(
    const StringRef name, const StringRef desc, const bool hasShort
) noexcept {
  String cmd = bold(cyan(name));
  if (hasShort) {
    cmd += ", ";
    cmd += bold(cyan(StringRef(name.data(), 1)));
  } else {
    // This coloring is for the alignment with std::setw later.
    cmd += bold(cyan("   "));
  }

  std::cout << "  " << std::left;
  if (shouldColor()) {
    std::cout << std::setw(44);
  } else {
    std::cout << std::setw(10);
  }
  std::cout << cmd << desc << '\n';
}

void
printGlobalOpts() noexcept {
  for (const auto& [shrt, lng, placeholder, desc] : GLOBAL_OPT_HELPS) {
    printOption(lng, shrt, desc, placeholder);
  }
}

bool
commandExists(const StringRef cmd) noexcept {
  String checkCmd = "command -v ";
  checkCmd += cmd;
  checkCmd += " >/dev/null 2>&1";
  return runCmd(checkCmd) == EXIT_SUCCESS;
}
