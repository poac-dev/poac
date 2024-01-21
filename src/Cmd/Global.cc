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
  std::cout << cyan("[OPTIONS]");
  if (!usage.empty()) {
    std::cout << " " << cyan(usage) << '\n';
  }
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

Subcmd&
Subcmd::setDesc(StringRef desc) noexcept {
  this->desc = desc;
  return *this;
}
Subcmd&
Subcmd::addOpt(Opt opt) noexcept {
  opts.emplace_back(opt);
  return *this;
}
Subcmd&
Subcmd::setArg(Arg arg) noexcept {
  this->arg = arg;
  return *this;
}

[[nodiscard]] int
Subcmd::noSuchArg(StringRef arg) const {
  Vec<StringRef> candidates;
  for (const auto& opt : GLOBAL_OPTS) {
    candidates.push_back(opt.lng);
    if (!opt.shrt.empty()) {
      candidates.push_back(opt.shrt);
    }
  }
  for (const auto& opt : opts) {
    candidates.push_back(opt.lng);
    if (!opt.shrt.empty()) {
      candidates.push_back(opt.shrt);
    }
  }

  String suggestion;
  if (const auto similar = findSimilarStr(arg, candidates)) {
    suggestion = "       Did you mean `" + String(similar.value()) + "`?\n\n";
  }
  Logger::error(
      "no such argument: `", arg, "`\n\n", suggestion, "       Run `poac help ",
      name, "` for a list of arguments"
  );
  return EXIT_FAILURE;
}

void
Subcmd::printHelp() const noexcept {
  std::cout << desc << '\n';
  std::cout << '\n';
  printUsage(name, arg.name);
  std::cout << '\n';
  printHeader("Options:");
  printGlobalOpts();
  for (const auto& opt : opts) {
    std::cout << opt;
  }
  if (!arg.name.empty()) {
    std::cout << '\n';
    printHeader("Arguments:");
    std::cout << "  " << arg.name;
    if (!arg.desc.empty()) {
      std::cout << '\t' << arg.desc;
    }
    std::cout << '\n';
  }
}
