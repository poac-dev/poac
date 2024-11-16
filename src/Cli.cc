#include "Cli.hpp"

#include "Algos.hpp"
#include "Logger.hpp"
#include "TermColor.hpp"

#include <algorithm>
#include <cstdlib>
#include <fmt/core.h>
#include <functional>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

static constinit const std::string_view PADDING = "  ";

static void
setOffset(const size_t offset) noexcept {
  std::cout << PADDING << std::left
            << std::setw(static_cast<int>(offset + PADDING.size()));
}

static void
printHeader(const std::string_view header) noexcept {
  std::cout << bold(green(header)) << '\n';
}

static void
printUsage(
    const std::string_view name, const std::string_view cmd,
    const std::string_view usage
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
addOptCandidates(
    std::vector<std::string_view>& candidates, const std::vector<Opt>& opts
) noexcept {
  for (const auto& opt : opts) {
    candidates.push_back(opt.mName);
    if (!opt.mShortName.empty()) {
      candidates.push_back(opt.mShortName);
    }
  }
}

size_t
calcOptMaxShortSize(const std::vector<Opt>& opts) noexcept {
  size_t maxShortSize = 0;
  for (const auto& opt : opts) {
    if (opt.mIsHidden) {
      // Hidden option should not affect maxShortSize.
      continue;
    }
    maxShortSize = std::max(maxShortSize, opt.mShortName.size());
  }
  return maxShortSize;
}

size_t
calcOptMaxOffset(
    const std::vector<Opt>& opts, const size_t maxShortSize
) noexcept {
  size_t maxOffset = 0;
  for (const auto& opt : opts) {
    if (opt.mIsHidden) {
      // Hidden option should not affect maxOffset.
      continue;
    }
    maxOffset = std::max(maxOffset, opt.leftSize(maxShortSize));
  }
  return maxOffset;
}

void
printOpts(
    const std::vector<Opt>& opts, const size_t maxShortSize,
    const size_t maxOffset
) noexcept {
  for (const auto& opt : opts) {
    if (opt.mIsHidden) {
      // We don't print hidden options.
      continue;
    }
    opt.print(maxShortSize, maxOffset);
  }
}

void
Opt::print(const size_t maxShortSize, size_t maxOffset) const noexcept {
  std::string option;
  if (!mShortName.empty()) {
    option += bold(cyan(mShortName));
    option += ", ";
    if (maxShortSize > mShortName.size()) {
      option += std::string(maxShortSize - mShortName.size(), ' ');
    }
  } else {
    // This coloring is for the alignment with std::setw later.
    option += bold(cyan(std::string(maxShortSize, ' ')));
    option += "  "; // ", "
  }
  option += bold(cyan(mName));
  option += ' ';
  option += cyan(mPlaceholder);

  if (shouldColor()) {
    // Color escape sequences are not visible but affect std::setw.
    constexpr size_t colorEscapeSeqLen = 31;
    maxOffset += colorEscapeSeqLen;
  }
  setOffset(maxOffset);
  std::cout << option << mDesc;
  if (!mDefaultVal.empty()) {
    std::cout << " [default: " << mDefaultVal << ']';
  }
  std::cout << '\n';
}

std::string
Arg::getLeft() const noexcept {
  if (mName.empty()) {
    return "";
  }

  std::string left;
  if (mRequired) {
    left += '<';
  } else {
    left += '[';
  }
  left += mName;
  if (mRequired) {
    left += '>';
  } else {
    left += ']';
  }
  if (mVariadic) {
    left += "...";
  }
  return cyan(left);
}
void
Arg::print(size_t maxOffset) const noexcept {
  const std::string left = getLeft();
  if (shouldColor()) {
    // Color escape sequences are not visible but affect std::setw.
    constexpr size_t colorEscapeSeqLen = 9;
    maxOffset += colorEscapeSeqLen;
  }
  setOffset(maxOffset);
  std::cout << left;
  if (!mDesc.empty()) {
    std::cout << mDesc;
  }
  std::cout << '\n';
}

Subcmd&
Subcmd::addOpt(Opt opt) noexcept {
  mLocalOpts.emplace_back(opt);
  return *this;
}
Subcmd&
Subcmd::setMainFn(std::function<int(std::span<const std::string_view>)> mainFn
) noexcept {
  mMainFn = std::move(mainFn);
  return *this;
}
Subcmd&
Subcmd::setGlobalOpts(const std::vector<Opt>& globalOpts) noexcept {
  mGlobalOpts = globalOpts;
  return *this;
}
std::string
Subcmd::getUsage() const noexcept {
  std::string str = bold(green("Usage: "));
  str += bold(cyan(mCmdName));
  str += ' ';
  str += bold(cyan(mName));
  str += ' ';
  str += cyan("[OPTIONS]");
  if (!mArg.mName.empty()) {
    str += ' ';
    str += cyan(mArg.getLeft());
  }
  return str;
}

[[nodiscard]] int
Subcmd::noSuchArg(std::string_view arg) const {
  std::vector<std::string_view> candidates;
  if (mGlobalOpts.has_value()) {
    addOptCandidates(candidates, mGlobalOpts.value());
  }
  addOptCandidates(candidates, mLocalOpts);

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
Subcmd::missingArgumentForOpt(const std::string_view arg) {
  logger::error("Missing argument for `", arg, "`");
  return EXIT_FAILURE;
}

size_t
Subcmd::calcMaxShortSize() const noexcept {
  size_t maxShortSize = 0;
  if (mGlobalOpts.has_value()) {
    maxShortSize =
        std::max(maxShortSize, calcOptMaxShortSize(mGlobalOpts.value()));
  }
  maxShortSize = std::max(maxShortSize, calcOptMaxShortSize(mLocalOpts));
  return maxShortSize;
}
size_t
Subcmd::calcMaxOffset(const size_t maxShortSize) const noexcept {
  size_t maxOffset = 0;
  if (mGlobalOpts.has_value()) {
    maxOffset = std::max(
        maxOffset, calcOptMaxOffset(mGlobalOpts.value(), maxShortSize)
    );
  }
  maxOffset = std::max(maxOffset, calcOptMaxOffset(mLocalOpts, maxShortSize));

  if (!mArg.mDesc.empty()) {
    // If args does not have a description, it is not necessary to consider
    // its length.
    maxOffset = std::max(maxOffset, mArg.leftSize());
  }
  return maxOffset;
}

void
Subcmd::printHelp() const noexcept {
  const size_t maxShortSize = calcMaxShortSize();
  const size_t maxOffset = calcMaxOffset(maxShortSize);

  std::cout << mDesc << '\n';
  std::cout << '\n';
  std::cout << getUsage() << "\n\n";

  printHeader("Options:");
  if (mGlobalOpts.has_value()) {
    printOpts(mGlobalOpts.value(), maxShortSize, maxOffset);
  }
  printOpts(mLocalOpts, maxShortSize, maxOffset);

  if (!mArg.mName.empty()) {
    std::cout << '\n';
    printHeader("Arguments:");
    mArg.print(maxOffset);
  }
}

void
Subcmd::print(size_t maxOffset) const noexcept {
  std::string cmdStr = bold(cyan(mName));
  if (hasShort()) {
    cmdStr += ", ";
    cmdStr += bold(cyan(mShortName));
  } else {
    // This coloring is for the alignment with std::setw later.
    cmdStr += bold(cyan("   "));
  }

  if (shouldColor()) {
    // Color escape sequences are not visible but affect std::setw.
    constexpr size_t colorEscapeSeqLen = 22;
    maxOffset += colorEscapeSeqLen;
  }
  setOffset(maxOffset);
  std::cout << cmdStr << mDesc << '\n';
}

Cli&
Cli::addSubcmd(const Subcmd& subcmd) noexcept {
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
  const_cast<Subcmd&>(subcmd).setCmdName(mName).setGlobalOpts(mGlobalOpts);

  mSubcmds.insert_or_assign(subcmd.mName, subcmd);
  if (subcmd.hasShort()) {
    mSubcmds.insert_or_assign(subcmd.mShortName, subcmd);
  }
  return *this;
}
Cli&
Cli::addOpt(Opt opt) noexcept {
  if (opt.mIsGlobal) {
    mGlobalOpts.emplace_back(opt);
  } else {
    mLocalOpts.emplace_back(opt);
  }
  return *this;
}

bool
Cli::hasSubcmd(std::string_view subcmd) const noexcept {
  return mSubcmds.contains(subcmd);
}

[[nodiscard]] int
Cli::noSuchArg(std::string_view arg) const {
  std::vector<std::string_view> candidates;
  for (const auto& cmd : mSubcmds) {
    candidates.push_back(cmd.second.mName);
    if (!cmd.second.mShortName.empty()) {
      candidates.push_back(cmd.second.mShortName);
    }
  }
  addOptCandidates(candidates, mGlobalOpts);
  addOptCandidates(candidates, mLocalOpts);

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
Cli::exec(
    const std::string_view subcmd, const std::span<const std::string_view> args
) const {
  return mSubcmds.at(subcmd).mMainFn(transformOptions(subcmd, args));
}

std::vector<std::string_view>
Cli::transformOptions(
    std::string_view subcmd, std::span<const std::string_view> args
) const {
  const Subcmd& cmd = mSubcmds.at(subcmd);
  std::vector<std::string_view> transformed;
  transformed.reserve(args.size());
  for (std::size_t argIdx = 0; argIdx < args.size(); ++argIdx) {
    const std::string_view arg = args[argIdx];

    if (arg.starts_with("--")) {
      if (auto pos = arg.find_first_of('='); pos != std::string_view::npos) {
        transformed.push_back(arg.substr(0, pos));
        transformed.push_back(arg.substr(pos + 1));
        continue;
      }
    } else if (arg.starts_with("-")) {
      std::string_view multioption = arg.substr(1);
      bool handled = false;
      for (std::size_t i = 0; i < multioption.size(); ++i) {
        const auto handle = [&](const std::span<const Opt> opts) {
          for (const Opt& opt : opts) {
            if (opt.mShortName.empty()) {
              continue;
            }
            if (opt.mShortName.substr(1) != multioption.substr(i, 1)) {
              continue;
            }
            transformed.push_back(opt.mShortName);
            // Placeholder is not empty means that this option takes a value.
            if (!opt.mPlaceholder.empty()) {
              if (i + 1 < multioption.size()) {
                // Handle concatenated value (like -j1)
                transformed.push_back(multioption.substr(i + 1));
              } else if (argIdx + 1 < args.size()
                         && !args[argIdx + 1].starts_with("-")) {
                // Handle space-separated value (like -j 1)
                transformed.push_back(args[++argIdx]
                ); // Consume the next argument as the option's value
              }
            }
            handled = true;
          }
        };
        if (cmd.mGlobalOpts) {
          handle(*cmd.mGlobalOpts);
        }
        handle(cmd.mLocalOpts);
      }
      if (handled) {
        continue;
      }
    }

    transformed.push_back(arg);
  }
  return transformed;
}

void
Cli::printSubcmdHelp(const std::string_view subcmd) const noexcept {
  mSubcmds.at(subcmd).printHelp();
}

size_t
Cli::calcMaxShortSize() const noexcept {
  // This is for printing the help message of the poac command itself.  So,
  // we don't need to consider the length of the subcommands' options.

  size_t maxShortSize = 0;
  maxShortSize = std::max(maxShortSize, calcOptMaxShortSize(mGlobalOpts));
  maxShortSize = std::max(maxShortSize, calcOptMaxShortSize(mLocalOpts));
  return maxShortSize;
}

size_t
Cli::calcMaxOffset(const size_t maxShortSize) const noexcept {
  size_t maxOffset = 0;
  maxOffset = std::max(maxOffset, calcOptMaxOffset(mGlobalOpts, maxShortSize));
  maxOffset = std::max(maxOffset, calcOptMaxOffset(mLocalOpts, maxShortSize));

  for (const auto& [name, cmd] : mSubcmds) {
    if (cmd.mIsHidden) {
      // Hidden command should not affect maxOffset.
      continue;
    }

    size_t offset = name.size(); // "build"
    if (!cmd.mShortName.empty()) {
      offset += 2; // ", "
      offset += cmd.mShortName.size(); // "b"
    }
    maxOffset = std::max(maxOffset, offset);
  }
  return maxOffset;
}

void
Cli::printAllSubcmds(const bool showHidden, size_t maxOffset) const noexcept {
  for (const auto& [name, cmd] : mSubcmds) {
    if (!showHidden && cmd.mIsHidden) {
      // Hidden command should not affect maxOffset if `showHidden` is false.
      continue;
    }

    size_t offset = name.size(); // "build"
    if (!cmd.mShortName.empty()) {
      offset += 2; // ", "
      offset += cmd.mShortName.size(); // "b"
    }
    maxOffset = std::max(maxOffset, offset);
  }

  for (const auto& [name, cmd] : mSubcmds) {
    if (!showHidden && cmd.mIsHidden) {
      // We don't print hidden subcommands if `showHidden` is false.
      continue;
    }
    if (cmd.hasShort() && name == cmd.mShortName) {
      // We don't print an abbreviation.
      continue;
    }
    cmd.print(maxOffset);
  }
}

void
Cli::printCmdHelp() const noexcept {
  // Print help message for poac itself
  const size_t maxShortSize = calcMaxShortSize();
  const size_t maxOffset = calcMaxOffset(maxShortSize);

  std::cout << mDesc << '\n';
  std::cout << '\n';
  printUsage(mName, "", cyan("[COMMAND]"));
  std::cout << '\n';

  printHeader("Options:");
  printOpts(mGlobalOpts, maxShortSize, maxOffset);
  printOpts(mLocalOpts, maxShortSize, maxOffset);
  std::cout << '\n';

  printHeader("Commands:");
  printAllSubcmds(false, maxOffset);

  const std::string dummyDesc = "See all commands with " + bold(cyan("--list"));
  Subcmd{ "..." }.setDesc(dummyDesc).print(maxOffset);

  std::cout
      << '\n'
      << fmt::format(
             "See '{} {} {}' for more information on a specific command.\n",
             bold(cyan(mName)), bold(cyan("help")), cyan("<command>")
         );
}

[[nodiscard]] int
Cli::printHelp(const std::span<const std::string_view> args) const noexcept {
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
