#include "Cli.hpp"

#include "Algos.hpp"
#include "Logger.hpp"
#include "Rustify.hpp"
#include "TermColor.hpp"

#include <algorithm>
#include <cstdlib>
#include <fmt/core.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
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
printUsage(
    const StringRef name, const StringRef cmd, const StringRef usage
) noexcept {
  std::cout << bold(green("Usage: ")) << bold(cyan(name)) << ' ';
  if (!cmd.empty()) {
    std::cout << bold(cyan(cmd)) << ' ';
  }
  std::cout << cyan("[OPTIONS]");
  if (!usage.empty()) {
    std::cout << " " << usage;
  }
  std::cout << '\n';
}

void
addOptCandidates(Vec<StringRef>& candidates, const Vec<Opt>& opts) noexcept {
  for (const auto& opt : opts) {
    candidates.push_back(opt.name);
    if (!opt.shortName.empty()) {
      candidates.push_back(opt.shortName);
    }
  }
}

usize
calcOptMaxShortSize(const Vec<Opt>& opts) noexcept {
  usize maxShortSize = 0;
  for (const auto& opt : opts) {
    if (opt.isHidden) {
      // Hidden option should not affect maxShortSize.
      continue;
    }
    maxShortSize = std::max(maxShortSize, opt.shortName.size());
  }
  return maxShortSize;
}

usize
calcOptMaxOffset(const Vec<Opt>& opts, const usize maxShortSize) noexcept {
  usize maxOffset = 0;
  for (const auto& opt : opts) {
    if (opt.isHidden) {
      // Hidden option should not affect maxOffset.
      continue;
    }
    maxOffset = std::max(maxOffset, opt.leftSize(maxShortSize));
  }
  return maxOffset;
}

void
printOpts(
    const Vec<Opt>& opts, const usize maxShortSize, const usize maxOffset
) noexcept {
  for (const auto& opt : opts) {
    if (opt.isHidden) {
      // We don't print hidden options.
      continue;
    }
    opt.print(maxShortSize, maxOffset);
  }
}

void
Opt::print(const usize maxShortSize, usize maxOffset) const noexcept {
  std::string option;
  if (!shortName.empty()) {
    option += bold(cyan(shortName));
    option += ", ";
    if (maxShortSize > shortName.size()) {
      option += std::string(maxShortSize - shortName.size(), ' ');
    }
  } else {
    // This coloring is for the alignment with std::setw later.
    option += bold(cyan(std::string(maxShortSize, ' ')));
    option += "  "; // ", "
  }
  option += bold(cyan(name));
  option += ' ';
  option += cyan(placeholder);

  if (shouldColor()) {
    // Color escape sequences are not visible but affect std::setw.
    constexpr usize colorEscapeSeqLen = 31;
    maxOffset += colorEscapeSeqLen;
  }
  setOffset(maxOffset);
  std::cout << option << desc;
  if (!defaultVal.empty()) {
    std::cout << " [default: " << defaultVal << ']';
  }
  std::cout << '\n';
}

std::string
Arg::getLeft() const noexcept {
  if (name.empty()) {
    return "";
  }

  std::string left;
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
  const std::string left = getLeft();
  if (shouldColor()) {
    // Color escape sequences are not visible but affect std::setw.
    constexpr usize colorEscapeSeqLen = 9;
    maxOffset += colorEscapeSeqLen;
  }
  setOffset(maxOffset);
  std::cout << left;
  if (!desc.empty()) {
    std::cout << desc;
  }
  std::cout << '\n';
}

Subcmd&
Subcmd::addOpt(Opt opt) noexcept {
  localOpts.emplace_back(opt);
  return *this;
}
Subcmd&
Subcmd::setMainFn(std::function<int(std::span<const StringRef>)> mainFn
) noexcept {
  this->mainFn = std::move(mainFn);
  return *this;
}
Subcmd&
Subcmd::setGlobalOpts(const Vec<Opt>& globalOpts) noexcept {
  this->globalOpts = globalOpts;
  return *this;
}
std::string
Subcmd::getUsage() const noexcept {
  std::string str = bold(green("Usage: "));
  str += bold(cyan(cmdName));
  str += ' ';
  str += bold(cyan(name));
  str += ' ';
  str += cyan("[OPTIONS]");
  if (!arg.name.empty()) {
    str += ' ';
    str += cyan(arg.getLeft());
  }
  return str;
}

[[nodiscard]] int
Subcmd::noSuchArg(StringRef arg) const {
  Vec<StringRef> candidates;
  if (globalOpts.has_value()) {
    addOptCandidates(candidates, globalOpts.value());
  }
  addOptCandidates(candidates, localOpts);

  std::string suggestion;
  if (const auto similar = findSimilarStr(arg, candidates)) {
    suggestion = bold(cyan("  Tip:")) + " did you mean '"
                 + bold(yellow(similar.value())) + "'?\n\n";
  }
  logger::error(
      "unexpected argument '", bold(yellow(arg)), "' found\n\n", suggestion,
      getUsage(), "\n\n", "For more information, try '", bold(cyan("--help")),
      '\''
  );
  return EXIT_FAILURE;
}

[[nodiscard]] int
Subcmd::missingArgumentForOpt(const StringRef arg) {
  logger::error("Missing argument for `", arg, "`");
  return EXIT_FAILURE;
}

usize
Subcmd::calcMaxShortSize() const noexcept {
  usize maxShortSize = 0;
  if (globalOpts.has_value()) {
    maxShortSize =
        std::max(maxShortSize, calcOptMaxShortSize(globalOpts.value()));
  }
  maxShortSize = std::max(maxShortSize, calcOptMaxShortSize(localOpts));
  return maxShortSize;
}
usize
Subcmd::calcMaxOffset(const usize maxShortSize) const noexcept {
  usize maxOffset = 0;
  if (globalOpts.has_value()) {
    maxOffset =
        std::max(maxOffset, calcOptMaxOffset(globalOpts.value(), maxShortSize));
  }
  maxOffset = std::max(maxOffset, calcOptMaxOffset(localOpts, maxShortSize));

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
  std::cout << getUsage() << "\n\n";

  printHeader("Options:");
  if (globalOpts.has_value()) {
    printOpts(globalOpts.value(), maxShortSize, maxOffset);
  }
  printOpts(localOpts, maxShortSize, maxOffset);

  if (!arg.name.empty()) {
    std::cout << '\n';
    printHeader("Arguments:");
    arg.print(maxOffset);
  }
}

void
Subcmd::print(usize maxOffset) const noexcept {
  std::string cmdStr = bold(cyan(name));
  if (hasShort()) {
    cmdStr += ", ";
    cmdStr += bold(cyan(shortName));
  } else {
    // This coloring is for the alignment with std::setw later.
    cmdStr += bold(cyan("   "));
  }

  if (shouldColor()) {
    // Color escape sequences are not visible but affect std::setw.
    constexpr usize colorEscapeSeqLen = 22;
    maxOffset += colorEscapeSeqLen;
  }
  setOffset(maxOffset);
  std::cout << cmdStr << desc << '\n';
}

Cli&
Cli::addSubcmd(const Subcmd& subcmd) noexcept {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  const_cast<Subcmd&>(subcmd).setCmdName(name).setGlobalOpts(globalOpts);

  subcmds.insert_or_assign(subcmd.name, subcmd);
  if (subcmd.hasShort()) {
    subcmds.insert_or_assign(subcmd.shortName, subcmd);
  }
  return *this;
}
Cli&
Cli::addOpt(Opt opt) noexcept {
  if (opt.isGlobal) {
    globalOpts.emplace_back(opt);
  } else {
    localOpts.emplace_back(opt);
  }
  return *this;
}

bool
Cli::hasSubcmd(StringRef subcmd) const noexcept {
  return subcmds.contains(subcmd);
}

[[nodiscard]] int
Cli::noSuchArg(StringRef arg) const {
  Vec<StringRef> candidates;
  for (const auto& cmd : subcmds) {
    candidates.push_back(cmd.second.name);
    if (!cmd.second.shortName.empty()) {
      candidates.push_back(cmd.second.shortName);
    }
  }
  addOptCandidates(candidates, globalOpts);
  addOptCandidates(candidates, localOpts);

  std::string suggestion;
  if (const auto similar = findSimilarStr(arg, candidates)) {
    suggestion = bold(cyan("  Tip:")) + " did you mean '"
                 + bold(yellow(similar.value())) + "'?\n\n";
  }
  logger::error(
      "unexpected argument '", bold(yellow(arg)), "' found\n\n", suggestion,
      "For a list of commands, try '", bold(cyan("poac help")), '\''
  );
  return EXIT_FAILURE;
}

[[nodiscard]] int
Cli::exec(const StringRef subcmd, const std::span<const StringRef> args) const {
  return subcmds.at(subcmd).mainFn(args);
}

void
Cli::printSubcmdHelp(const StringRef subcmd) const noexcept {
  subcmds.at(subcmd).printHelp();
}

usize
Cli::calcMaxShortSize() const noexcept {
  // This is for printing the help message of the poac command itself.  So,
  // we don't need to consider the length of the subcommands' options.

  usize maxShortSize = 0;
  maxShortSize = std::max(maxShortSize, calcOptMaxShortSize(globalOpts));
  maxShortSize = std::max(maxShortSize, calcOptMaxShortSize(localOpts));
  return maxShortSize;
}

usize
Cli::calcMaxOffset(const usize maxShortSize) const noexcept {
  usize maxOffset = 0;
  maxOffset = std::max(maxOffset, calcOptMaxOffset(globalOpts, maxShortSize));
  maxOffset = std::max(maxOffset, calcOptMaxOffset(localOpts, maxShortSize));

  for (const auto& [name, cmd] : subcmds) {
    if (cmd.isHidden) {
      // Hidden command should not affect maxOffset.
      continue;
    }

    usize offset = name.size(); // "build"
    if (!cmd.shortName.empty()) {
      offset += 2; // ", "
      offset += cmd.shortName.size(); // "b"
    }
    maxOffset = std::max(maxOffset, offset);
  }
  return maxOffset;
}

void
Cli::printAllSubcmds(const bool showHidden, usize maxOffset) const noexcept {
  for (const auto& [name, cmd] : subcmds) {
    if (!showHidden && cmd.isHidden) {
      // Hidden command should not affect maxOffset if `showHidden` is false.
      continue;
    }

    usize offset = name.size(); // "build"
    if (!cmd.shortName.empty()) {
      offset += 2; // ", "
      offset += cmd.shortName.size(); // "b"
    }
    maxOffset = std::max(maxOffset, offset);
  }

  for (const auto& [name, cmd] : subcmds) {
    if (!showHidden && cmd.isHidden) {
      // We don't print hidden subcommands if `showHidden` is false.
      continue;
    }
    if (cmd.hasShort() && name == cmd.shortName) {
      // We don't print an abbreviation.
      continue;
    }
    cmd.print(maxOffset);
  }
}

void
Cli::printCmdHelp() const noexcept {
  // Print help message for poac itself
  const usize maxShortSize = calcMaxShortSize();
  const usize maxOffset = calcMaxOffset(maxShortSize);

  std::cout << desc << '\n';
  std::cout << '\n';
  printUsage(name, "", cyan("[COMMAND]"));
  std::cout << '\n';

  printHeader("Options:");
  printOpts(globalOpts, maxShortSize, maxOffset);
  printOpts(localOpts, maxShortSize, maxOffset);
  std::cout << '\n';

  printHeader("Commands:");
  printAllSubcmds(false, maxOffset);

  const std::string dummyDesc = "See all commands with " + bold(cyan("--list"));
  Subcmd{ "..." }.setDesc(dummyDesc).print(maxOffset);

  std::cout
      << '\n'
      << fmt::format(
             "See '{} {} {}' for more information on a specific command.\n",
             bold(cyan(name)), bold(cyan("help")), cyan("<command>")
         );
}

[[nodiscard]] int
Cli::printHelp(const std::span<const StringRef> args) const noexcept {
  // Parse args
  for (auto itr = args.begin(); itr != args.end(); ++itr) {
    if (const auto res = handleGlobalOpts(itr, args.end(), "help")) {
      if (res.value() == Cli::CONTINUE) {
        continue;
      } else {
        return res.value();
      }
    } else if (hasSubcmd(*itr)) {
      printSubcmdHelp(*itr);
      return EXIT_SUCCESS;
    } else {
      // TODO: Currently assumes that `help` does not implement any additional
      // options since we are using `noSuchArg` instead of
      // `helpCmd.noSuchArg`. But we want to consider subcommands as well for
      // suggestion.
      return noSuchArg(*itr);
    }
  }

  // Print help message for poac itself
  printCmdHelp();
  return EXIT_SUCCESS;
}
