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

struct Opt {
  StringRef name;
  StringRef shortName;
  StringRef desc;
  StringRef placeholder;
  StringRef defaultVal;
  bool isGlobal = false;

  constexpr Opt() noexcept = default;
  constexpr ~Opt() noexcept = default;
  constexpr Opt(const Opt&) noexcept = default;
  constexpr Opt(Opt&&) noexcept = default;
  constexpr Opt& operator=(const Opt&) noexcept = default;
  constexpr Opt& operator=(Opt&&) noexcept = default;

  explicit constexpr Opt(const StringRef name) noexcept : name(name) {}

  inline constexpr Opt setShort(const StringRef shortName) noexcept {
    this->shortName = shortName;
    return *this;
  }
  inline constexpr Opt setDesc(const StringRef desc) noexcept {
    this->desc = desc;
    return *this;
  }
  inline constexpr Opt setPlaceholder(const StringRef placeholder) noexcept {
    this->placeholder = placeholder;
    return *this;
  }
  inline constexpr Opt setDefault(const StringRef defaultVal) noexcept {
    this->defaultVal = defaultVal;
    return *this;
  }
  inline constexpr Opt setGlobal(const bool isGlobal) noexcept {
    this->isGlobal = isGlobal;
    return *this;
  }

  /// Size of `-c, --color <WHEN>` without color.
  usize leftSize(usize maxShortSize) const noexcept;
  void print(usize maxShortSize, usize maxOffset) const noexcept;
};

inline constinit const Arr<Opt, 4> GLOBAL_OPTS{
  Opt{ "--verbose" }.setShort("-v").setDesc("Use verbose output"),
  Opt{ "--quiet" }.setShort("-q").setDesc("Do not print poac log messages"),
  Opt{ "--color" }
      .setDesc("Coloring: auto, always, never")
      .setPlaceholder("<WHEN>"),
  Opt{ "--help" }.setShort("-h").setDesc("Print help"),
};

struct Arg {
  StringRef name;
  StringRef desc;
  bool required = true;
  bool variadic = false;

  constexpr Arg() noexcept = default;
  constexpr ~Arg() noexcept = default;
  constexpr Arg(const Arg&) noexcept = default;
  constexpr Arg(Arg&&) noexcept = default;
  constexpr Arg& operator=(const Arg&) noexcept = default;
  constexpr Arg& operator=(Arg&&) noexcept = default;

  explicit constexpr Arg(const StringRef name) noexcept : name(name) {}

  inline constexpr Arg setDesc(const StringRef desc) noexcept {
    this->desc = desc;
    return *this;
  }
  inline constexpr Arg setRequired(const bool required) noexcept {
    this->required = required;
    return *this;
  }
  inline constexpr Arg setVariadic(const bool variadic) noexcept {
    this->variadic = variadic;
    return *this;
  }

  /// Size of left side of the help message.
  usize leftSize() const noexcept;
  String getLeft() const noexcept;
  void print(usize maxOffset) const noexcept;
};

struct Subcmd {
  StringRef name;
  StringRef shortName;
  StringRef desc;
  Vec<Opt> opts;
  Arg arg;
  Fn<int(std::span<const StringRef>)> mainFn;

  Subcmd() noexcept = delete;
  ~Subcmd() noexcept = default;
  Subcmd(const Subcmd&) noexcept = default;
  Subcmd(Subcmd&&) noexcept = default;
  Subcmd& operator=(const Subcmd&) noexcept = default;
  Subcmd& operator=(Subcmd&&) noexcept = default;

  explicit Subcmd(const StringRef name) noexcept : name(name) {}

  Subcmd& setDesc(StringRef desc) noexcept;
  StringRef getDesc() const noexcept;
  Subcmd& setShort(StringRef shortName) noexcept;
  StringRef getShort() const noexcept;
  bool hasShort() const noexcept;
  Subcmd& addOpt(const Opt& opt) noexcept;
  Subcmd& setArg(const Arg& arg) noexcept;
  Subcmd& setMainFn(Fn<int(std::span<const StringRef>)> mainFn) noexcept;

  String getUsage() const noexcept;
  [[nodiscard]] int noSuchArg(StringRef arg) const;
  void printHelp() const noexcept;

private:
  usize calcMaxShortSize() const noexcept;
  /// Calculate the maximum length of the left side of the helps to align the
  /// descriptions with 2 spaces.
  usize calcMaxOffset(usize maxShortSize) const noexcept;
};

class Command {
public:
  StringRef name;
  StringRef desc;
  HashMap<StringRef, Subcmd> subcmds; // TODO: should not expose
  Vec<Opt> opts;

  Command() noexcept = delete;
  ~Command() noexcept = default;
  Command(const Command&) noexcept = default;
  Command(Command&&) noexcept = default;
  Command& operator=(const Command&) noexcept = default;
  Command& operator=(Command&&) noexcept = default;

  explicit Command(const StringRef name) noexcept : name(name) {}

  Command& setDesc(StringRef desc) noexcept;
  //   StringRef getDesc() const noexcept;
  Command& addSubcmd(const Subcmd& subcmd) noexcept;
  Command& addOpt(const Opt& opt) noexcept;

  bool hasSubcmd(StringRef subcmd) const noexcept;
  //   String getUsage() const noexcept;
  [[nodiscard]] int noSuchArg(StringRef arg) const;
  void printHelp(StringRef subcmd) const noexcept;
  [[nodiscard]] int
  exec(StringRef subcmd, std::span<const StringRef> args) const;

private:
  //   usize calcMaxShortSize() const noexcept;
  //   usize calcMaxOffset(usize maxShortSize) const noexcept;
};

void printHeader(StringRef header) noexcept;
void printUsage(StringRef cmd, StringRef usage) noexcept;
void printCommand(StringRef name, const Subcmd& cmd, usize maxOffset) noexcept;
void printGlobalOpts(usize maxShortSize, usize maxOffset) noexcept;
