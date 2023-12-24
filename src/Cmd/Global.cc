#include "Global.hpp"

#include "../Rustify.hpp"
#include "../TermColor.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>

void printHeader(StringRef header) noexcept {
  std::cout << bold(green(header)) << '\n';
}

void printUsage(StringRef cmd, StringRef usage) noexcept {
  std::cout << bold(green("Usage: ")) << bold(cyan("poac "));
  if (!cmd.empty()) {
    std::cout << bold(cyan(cmd)) << ' ';
  }
  std::cout << cyan(usage) << '\n';
}

void printOption(
    StringRef lng, StringRef shrt, StringRef desc, StringRef placeholder
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

void printCommand(StringRef name, StringRef desc) noexcept {
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

bool commandExists(StringRef cmd) noexcept {
  String checkCmd = "command -v ";
  checkCmd += cmd;
  checkCmd += " >/dev/null 2>&1";
  return std::system(checkCmd.c_str()) == 0;
}
