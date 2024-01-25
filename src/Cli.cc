#include "Cli.hpp"

#include "Algos.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"
#include "TermColor.hpp"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <utility>

static constinit const StringRef PADDING = "  ";

static void
setOffset(const usize offset) noexcept {
  std::cout << PADDING << std::left
            << std::setw(static_cast<int>(offset + PADDING.size()));
}

static void
printHeader(const StringRef header) noexcept {
  std::cout << bold(green(header)) << '\n';
}

static void
printUsage(const StringRef cmd, const StringRef usage) noexcept {
  std::cout << bold(green("Usage: ")) << bold(cyan("poac "));
  if (!cmd.empty()) {
    std::cout << bold(cyan(cmd)) << ' ';
  }
  std::cout << cyan("[OPTIONS]");
  if (!usage.empty()) {
    std::cout << " " << usage;
  }
  std::cout << '\n';
}

usize
Opt::leftSize(const usize maxShortSize) const noexcept {
  // shrt.size() = ?
  // `, `.size() = 2
  // lng.size() = ?
  // ` `.size() = 1
  // placeholder.size() = ?
  return 3 + maxShortSize + name.size() + placeholder.size();
}

void
Opt::print(const usize maxShortSize, usize maxOffset) const noexcept {
  String option;
  if (!shortName.empty()) {
    option += bold(cyan(shortName));
    option += ", ";
    if (maxShortSize > shortName.size()) {
      option += String(maxShortSize - shortName.size(), ' ');
    }
  } else {
    // This coloring is for the alignment with std::setw later.
    option += bold(cyan(String(maxShortSize, ' ')));
    option += "  "; // ", "
  }
  option += bold(cyan(name));
  option += ' ';
  option += cyan(placeholder);

  if (shouldColor()) {
    maxOffset += 43; // invisible color escape sequences.
  }
  setOffset(maxOffset);
  std::cout << option << desc;
  if (!defaultVal.empty()) {
    std::cout << " [default: " << defaultVal << ']';
  }
  std::cout << '\n';
}

usize
Arg::leftSize() const noexcept {
  return name.size();
}
String
Arg::getLeft() const noexcept {
  if (name.empty()) {
    return "";
  }

  String left;
  if (required) {
    left += '<';
  } else {
    left += '[';
  }
  left += name;
  if (required) {
    left += '>';
  } else {
    left += ']';
  }
  if (variadic) {
    left += "...";
  }
  return cyan(left);
}
void
Arg::print(usize maxOffset) const noexcept {
  const String left = getLeft();
  if (shouldColor()) {
    maxOffset += 9; // invisible color escape sequences.
  }
  setOffset(maxOffset);
  std::cout << left;
  if (!desc.empty()) {
    std::cout << desc;
  }
  std::cout << '\n';
}

Subcmd&
Subcmd::setDesc(StringRef desc) noexcept {
  this->desc = desc;
  return *this;
}
Subcmd&
Subcmd::setShort(StringRef shortName) noexcept {
  this->shortName = shortName;
  return *this;
}
bool
Subcmd::hasShort() const noexcept {
  return !shortName.empty();
}
Subcmd&
Subcmd::addOpt(Opt opt) noexcept {
  localOpts.emplace_back(opt);
  return *this;
}
Subcmd&
Subcmd::setArg(Arg arg) noexcept {
  this->arg = arg;
  return *this;
}
Subcmd&
Subcmd::setMainFn(Fn<int(std::span<const StringRef>)> mainFn) noexcept {
  this->mainFn = std::move(mainFn);
  return *this;
}

Subcmd&
Subcmd::setGlobalOpts(Vec<Opt>* globalOpts) noexcept {
  this->globalOpts = globalOpts;
  return *this;
}
String
Subcmd::getUsage() const noexcept {
  String str = bold(green("Usage: "));
  str += bold(cyan("poac "));
  str += bold(cyan(name));
  str += ' ';
  str += cyan("[OPTIONS]");
  if (!arg.name.empty()) {
    str += ' ';
    str += cyan(arg.name);
  }
  return str;
}

[[nodiscard]] int
Subcmd::noSuchArg(StringRef arg) const {
  Vec<StringRef> candidates;
  if (globalOpts) {
    for (const auto& opt : *globalOpts) {
      candidates.push_back(opt.name);
      if (!opt.shortName.empty()) {
        candidates.push_back(opt.shortName);
      }
    }
  }
  for (const auto& opt : localOpts) {
    candidates.push_back(opt.name);
    if (!opt.shortName.empty()) {
      candidates.push_back(opt.shortName);
    }
  }

  String suggestion;
  if (const auto similar = findSimilarStr(arg, candidates)) {
    suggestion = bold(cyan("  Tip:")) + " did you mean '"
                 + bold(yellow(similar.value())) + "'?\n\n";
  }
  Logger::error(
      "unexpected argument '", bold(yellow(arg)), "' found\n\n", suggestion,
      getUsage(), "\n\n", "For more information, try '", bold(cyan("--help")),
      '\''
  );
  return EXIT_FAILURE;
}

usize
Subcmd::calcMaxShortSize() const noexcept {
  usize maxShortSize = 0;
  if (globalOpts) {
    for (const auto& opt : *globalOpts) {
      maxShortSize = std::max(maxShortSize, opt.shortName.size());
    }
  }
  for (const auto& opt : localOpts) {
    maxShortSize = std::max(maxShortSize, opt.shortName.size());
  }
  return maxShortSize;
}
usize
Subcmd::calcMaxOffset(const usize maxShortSize) const noexcept {
  usize maxOffset = 0;
  if (globalOpts) {
    for (const auto& opt : *globalOpts) {
      maxOffset = std::max(maxOffset, opt.leftSize(maxShortSize));
    }
  }
  for (const auto& opt : localOpts) {
    maxOffset = std::max(maxOffset, opt.leftSize(maxShortSize));
  }
  if (!arg.desc.empty()) {
    // If args does not have a description, it is not necessary to consider
    // its length.
    maxOffset = std::max(maxOffset, arg.leftSize());
  }
  return maxOffset;
}

void
Subcmd::printHelp() const noexcept {
  const usize maxShortSize = calcMaxShortSize();
  const usize maxOffset = calcMaxOffset(maxShortSize);

  std::cout << desc << '\n';
  std::cout << '\n';

  printUsage(name, arg.getLeft());
  std::cout << '\n';

  printHeader("Options:");
  if (globalOpts) {
    for (const auto& opt : *globalOpts) {
      opt.print(maxShortSize, maxOffset);
    }
  }
  for (const auto& opt : localOpts) {
    opt.print(maxShortSize, maxOffset);
  }

  if (!arg.name.empty()) {
    std::cout << '\n';
    printHeader("Arguments:");
    arg.print(maxOffset);
  }
}

void
Subcmd::print(usize maxOffset) const noexcept {
  String cmdStr = bold(cyan(name));
  if (hasShort()) {
    cmdStr += ", ";
    cmdStr += bold(cyan(shortName));
  } else {
    // This coloring is for the alignment with std::setw later.
    cmdStr += bold(cyan("   "));
  }

  if (shouldColor()) {
    maxOffset += 34; // invisible color escape sequences.
  }
  setOffset(maxOffset);
  std::cout << cmdStr << desc << '\n';
}

Command&
Command::setDesc(StringRef desc) noexcept {
  this->desc = desc;
  return *this;
}
Command&
Command::addSubcmd(Subcmd subcmd) noexcept {
  subcmd.setGlobalOpts(&globalOpts);

  subcmds.emplace(subcmd.name, subcmd);
  if (subcmd.hasShort()) {
    subcmds.emplace(subcmd.shortName, subcmd);
  }
  return *this;
}
Command&
Command::addOpt(Opt opt) noexcept {
  if (opt.isGlobal) {
    globalOpts.emplace_back(opt);
  } else {
    localOpts.emplace_back(opt);
  }
  return *this;
}

bool
Command::hasSubcmd(StringRef subcmd) const noexcept {
  return subcmds.contains(subcmd);
}

[[nodiscard]] int
Command::noSuchArg(StringRef arg) const {
  Vec<StringRef> candidates;
  for (const auto& cmd : subcmds) {
    candidates.push_back(cmd.second.name);
    if (!cmd.second.shortName.empty()) {
      candidates.push_back(cmd.second.shortName);
    }
  }
  for (const auto& opt : globalOpts) {
    candidates.push_back(opt.name);
    if (!opt.shortName.empty()) {
      candidates.push_back(opt.shortName);
    }
  }
  for (const auto& opt : localOpts) {
    candidates.push_back(opt.name);
    if (!opt.shortName.empty()) {
      candidates.push_back(opt.shortName);
    }
  }

  String suggestion;
  if (const auto similar = findSimilarStr(arg, candidates)) {
    suggestion = bold(cyan("  Tip:")) + " did you mean '"
                 + bold(yellow(similar.value())) + "'?\n\n";
  }
  Logger::error(
      "unexpected argument '", bold(yellow(arg)), "' found\n\n", suggestion,
      "For a list of commands, try '", bold(cyan("poac help")), '\''
  );
  return EXIT_FAILURE;
}

[[nodiscard]] int
Command::exec(const StringRef subcmd, const std::span<const StringRef> args)
    const {
  return subcmds.at(subcmd).mainFn(args);
}

void
Command::printSubcmdHelp(const StringRef subcmd) const noexcept {
  subcmds.at(subcmd).printHelp();
}

usize
Command::calcMaxShortSize() const noexcept {
  usize maxShortSize = 0;
  for (const auto& opt : globalOpts) {
    maxShortSize = std::max(maxShortSize, opt.shortName.size());
  }
  for (const auto& opt : localOpts) {
    maxShortSize = std::max(maxShortSize, opt.shortName.size());
  }
  return maxShortSize;
}

usize
Command::calcMaxOffset(const usize maxShortSize) const noexcept {
  usize maxOffset = 0;
  for (const auto& [name, cmd] : subcmds) {
    maxOffset = std::max(maxOffset, cmd.calcMaxOffset(maxShortSize));
  }
  return maxOffset;
}

void
Command::printHelp() const noexcept {
  const usize maxShortSize = 0; // TODO: calcMaxShortSize();
  const usize maxOffset = 0; // TODO: calcMaxOffset(maxShortSize);

  std::cout << desc << '\n';
  std::cout << '\n';
  printUsage("", cyan("[COMMAND]"));
  std::cout << '\n';

  printHeader("Options:");
  for (const auto& opt : globalOpts) {
    opt.print(maxShortSize, maxOffset);
  }
  for (const auto& opt : localOpts) {
    opt.print(maxShortSize, maxOffset);
  }
  std::cout << '\n';

  printHeader("Commands:");
  for (const auto& [name, cmd] : subcmds) {
    if (cmd.hasShort() && name == cmd.shortName) {
      // We don't print an abbreviation.
      return;
    }
    cmd.print(maxOffset);
  }
}

// TODO: This should be automatically generated.
int
helpMain(const std::span<const StringRef> args) noexcept {
  // Parse args
  for (usize i = 0; i < args.size(); ++i) {
    const StringRef arg = args[i];
    HANDLE_GLOBAL_OPTS({ { "help" } })

    else if (getCmd().hasSubcmd(arg)) {
      getCmd().printSubcmdHelp(arg);
      return EXIT_SUCCESS;
    }
    else {
      // TODO: should take a list of commands as well
      return helpCmd.noSuchArg(arg);
    }
  }

  // Print help message for poac itself
  getCmd().printHelp();
  return EXIT_SUCCESS;
}
