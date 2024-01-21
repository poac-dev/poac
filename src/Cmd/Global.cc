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
  for (const auto& opt : GLOBAL_OPTS) {
    std::cout << opt;
  }
}

std::ostream&
operator<<(std::ostream& os, const Opt& opt) noexcept {
  String option;
  if (!opt.shrt.empty()) {
    option += bold(cyan(opt.shrt));
    option += ", ";
  } else {
    // This coloring is for the alignment with std::setw later.
    option += bold(cyan("    "));
  }
  option += bold(cyan(opt.lng));
  option += ' ';
  option += cyan(opt.placeholder);

  os << "  " << std::left;
  if (shouldColor()) {
    os << std::setw(69);
  } else {
    os << std::setw(26);
  }
  os << option << opt.desc;
  if (!opt.defaultVal.empty()) {
    os << " [default: " << opt.defaultVal << ']';
  }
  os << '\n';
  return os;
}

bool
commandExists(const StringRef cmd) noexcept {
  String checkCmd = "command -v ";
  checkCmd += cmd;
  checkCmd += " >/dev/null 2>&1";
  return runCmd(checkCmd) == EXIT_SUCCESS;
}
