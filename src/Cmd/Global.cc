#include "Global.hpp"

#include "../Algos.hpp"
#include "../Rustify.hpp"
#include "../TermColor.hpp"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>

bool
commandExists(const StringRef cmd) noexcept {
  String checkCmd = "command -v ";
  checkCmd += cmd;
  checkCmd += " >/dev/null 2>&1";
  return runCmd(checkCmd) == EXIT_SUCCESS;
}

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
    std::cout << " " << cyan(usage);
  }
  std::cout << '\n';
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
printGlobalOpts(const usize maxOptLen) noexcept {
  for (const auto& opt : GLOBAL_OPTS) {
    opt.print(maxOptLen);
  }
}

usize
Opt::leftSize() const noexcept {
  // shrt.size() = 2
  // `, `.size() = 2
  // lng.size() = ?
  // ` `.size() = 1
  // placeholder.size() = ?
  return 5 + lng.size() + placeholder.size();
}

void
Opt::print(usize maxLeftSize) const noexcept {
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
  maxLeftSize += 2; // spaces between left and desc.
  if (shouldColor()) {
    maxLeftSize += 43; // invisible color escape sequences.
  }
  std::cout << std::setw(static_cast<int>(maxLeftSize)) << option << desc;
  if (!defaultVal.empty()) {
    std::cout << " [default: " << defaultVal << ']';
  }
  std::cout << '\n';
}

Subcmd&
Subcmd::setDesc(StringRef desc) noexcept {
  this->desc = desc;
  return *this;
}
Subcmd&
Subcmd::addOpt(const Opt& opt) noexcept {
  opts.emplace_back(opt);
  return *this;
}
Subcmd&
Subcmd::setArg(const Arg& arg) noexcept {
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

  // Calculate the maximum length of the left side of the options to align the
  // descriptions with 2 spaces.
  usize maxLeftSize = 0;
  for (const auto& opt : GLOBAL_OPTS) {
    maxLeftSize = std::max(maxLeftSize, opt.leftSize());
  }
  for (const auto& opt : opts) {
    maxLeftSize = std::max(maxLeftSize, opt.leftSize());
  }

  printHeader("Options:");
  printGlobalOpts(maxLeftSize);
  for (const auto& opt : opts) {
    opt.print(maxLeftSize);
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
