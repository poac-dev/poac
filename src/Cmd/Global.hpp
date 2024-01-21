// Global options and helper functions for commands.

#pragma once

#include "../Algos.hpp"
#include "../Logger.hpp"
#include "../Rustify.hpp"
#include "../TermColor.hpp"
#include "Help.hpp"

#include <cstdlib>
#include <ostream>
#include <tuple>

#define HANDLE_GLOBAL_OPTS(HELP_ARGS)                  \
  if (arg == "-h" || arg == "--help") {                \
    return helpMain(HELP_ARGS);                        \
  } else if (arg == "-v" || arg == "--verbose") {      \
    Logger::setLevel(LogLevel::debug);                 \
  } else if (arg == "-q" || arg == "--quiet") {        \
    Logger::setLevel(LogLevel::off);                   \
  } else if (arg == "--color") {                       \
    if (i + 1 < args.size()) {                         \
      setColorMode(args[++i]);                         \
    } else {                                           \
      Logger::error("missing argument for `--color`"); \
      return EXIT_FAILURE;                             \
    }                                                  \
  }

void printHeader(StringRef header) noexcept;
void printUsage(StringRef cmd, StringRef usage) noexcept;
void printCommand(StringRef name, StringRef desc, bool hasShort) noexcept;
void printGlobalOpts() noexcept;

bool commandExists(StringRef cmd) noexcept;

struct Opt {
  StringRef lng;
  StringRef shrt;
  StringRef desc;
  StringRef placeholder;
  StringRef defaultVal;

  constexpr Opt() noexcept = default;
  constexpr ~Opt() noexcept = default;
  constexpr Opt(const Opt&) noexcept = default;
  constexpr Opt(Opt&&) noexcept = default;
  constexpr Opt& operator=(const Opt&) noexcept = default;
  constexpr Opt& operator=(Opt&&) noexcept = default;

  explicit constexpr Opt(StringRef lng, StringRef shrt = "") noexcept
      : lng(lng), shrt(shrt) {}

  inline constexpr Opt setDesc(StringRef desc) noexcept {
    this->desc = desc;
    return *this;
  }
  inline constexpr Opt setPlaceholder(StringRef placeholder) noexcept {
    this->placeholder = placeholder;
    return *this;
  }
  inline constexpr Opt setDefault(StringRef defaultVal) noexcept {
    this->defaultVal = defaultVal;
    return *this;
  }
};
std::ostream& operator<<(std::ostream& os, const Opt& opt) noexcept;

static inline constinit const Arr<Opt, 4> GLOBAL_OPTS{
  Opt{ "--verbose", "-v" }.setDesc("Use verbose output"),
  Opt{ "--quiet", "-q" }.setDesc("Do not print poac log messages"),
  Opt{ "--color" }
      .setDesc("Coloring: auto, always, never")
      .setPlaceholder("<WHEN>"),
  Opt{ "--help", "-h" }.setDesc("Print help"),
};

template <usize NumOpts>
struct Subcmd {
  StringRef name;
  StringRef desc;
  StringRef usage;
  Arr<Opt, NumOpts> opts{};
  usize optPos = 0;
  StringRef args;

  constexpr Subcmd() noexcept = delete;
  constexpr ~Subcmd() noexcept = default;
  constexpr Subcmd(const Subcmd&) noexcept = default;
  constexpr Subcmd(Subcmd&&) noexcept = default;
  constexpr Subcmd& operator=(const Subcmd&) noexcept = default;
  constexpr Subcmd& operator=(Subcmd&&) noexcept = default;

  explicit constexpr Subcmd(StringRef name) noexcept : name(name) {}

  inline constexpr Subcmd setDesc(StringRef desc) noexcept {
    this->desc = desc;
    return *this;
  }
  inline constexpr Subcmd setUsage(StringRef usage) noexcept {
    this->usage = usage;
    return *this;
  }
  inline constexpr Subcmd addOpt(Opt opt) noexcept {
    opts.at(optPos++) = opt;
    return *this;
  }
  inline constexpr Subcmd setArgs(StringRef args) noexcept {
    this->args = args;
    return *this;
  }

  [[nodiscard]] inline int noSuchArg(StringRef arg) const {
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
        "no such argument: `", arg, "`\n\n", suggestion,
        "       Run `poac help ", name, "` for a list of arguments"
    );
    return EXIT_FAILURE;
  }

  inline void printHelp() const noexcept {
    std::cout << desc << '\n';
    std::cout << '\n';
    printUsage(name, usage);
    std::cout << '\n';
    printHeader("Options:");
    printGlobalOpts();
    for (const auto& opt : opts) {
      std::cout << opt;
    }
    if (!args.empty()) {
      std::cout << '\n';
      printHeader("Arguments:");
      std::cout << "  " << args << '\n';
    }
  }
};
