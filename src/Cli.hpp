#pragma once

#include "Rustify.hpp"

#include <cstdlib>
#include <span>
#include <tuple>

#define HANDLE_GLOBAL_OPTS(HELP_ARGS)                  \
  if (arg == "-h" || arg == "--help") {                \
    return getCmd().printHelp(HELP_ARGS);              \
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

class Opt;
class Arg;
class Subcmd;
class Command;

// Defined in main.cc
const Command& getCmd() noexcept;

class Opt {
  friend class Subcmd;
  friend class Command;

  StringRef name;
  StringRef shortName;
  StringRef desc;
  StringRef placeholder;
  StringRef defaultVal;
  bool isGlobal = false;

public:
  constexpr Opt() noexcept = default;
  constexpr ~Opt() noexcept = default;
  constexpr Opt(const Opt&) noexcept = default;
  constexpr Opt(Opt&&) noexcept = default;
  constexpr Opt& operator=(const Opt&) noexcept = default;
  constexpr Opt& operator=(Opt&&) noexcept = default;

  explicit constexpr Opt(const StringRef name) noexcept : name(name) {}

  constexpr Opt setShort(const StringRef shortName) noexcept {
    this->shortName = shortName;
    return *this;
  }
  constexpr Opt setDesc(const StringRef desc) noexcept {
    this->desc = desc;
    return *this;
  }
  constexpr Opt setPlaceholder(const StringRef placeholder) noexcept {
    this->placeholder = placeholder;
    return *this;
  }
  constexpr Opt setDefault(const StringRef defaultVal) noexcept {
    this->defaultVal = defaultVal;
    return *this;
  }
  constexpr Opt setGlobal(const bool isGlobal) noexcept {
    this->isGlobal = isGlobal;
    return *this;
  }

private:
  /// Size of `-c, --color <WHEN>` without color.
  usize leftSize(usize maxShortSize) const noexcept;
  void print(usize maxShortSize, usize maxOffset) const noexcept;
};

class Arg {
  friend class Subcmd;

  StringRef name;
  StringRef desc;
  bool required = true;
  bool variadic = false;

public:
  constexpr Arg() noexcept = default;
  constexpr ~Arg() noexcept = default;
  constexpr Arg(const Arg&) noexcept = default;
  constexpr Arg(Arg&&) noexcept = default;
  constexpr Arg& operator=(const Arg&) noexcept = default;
  constexpr Arg& operator=(Arg&&) noexcept = default;

  explicit constexpr Arg(const StringRef name) noexcept : name(name) {}

  constexpr Arg setDesc(const StringRef desc) noexcept {
    this->desc = desc;
    return *this;
  }
  constexpr Arg setRequired(const bool required) noexcept {
    this->required = required;
    return *this;
  }
  constexpr Arg setVariadic(const bool variadic) noexcept {
    this->variadic = variadic;
    return *this;
  }

private:
  /// Size of left side of the help message.
  usize leftSize() const noexcept;
  String getLeft() const noexcept;
  void print(usize maxOffset) const noexcept;
};

class Subcmd {
  friend class Command;

  StringRef name;
  StringRef shortName;
  StringRef desc;
  Option<Vec<Opt>> globalOpts = None;
  Vec<Opt> localOpts;
  Arg arg;
  Fn<int(std::span<const StringRef>)> mainFn;

public:
  Subcmd() noexcept = delete;
  ~Subcmd() noexcept = default;
  Subcmd(const Subcmd&) noexcept = default;
  Subcmd(Subcmd&&) noexcept = default;
  Subcmd& operator=(const Subcmd&) noexcept = default;
  Subcmd& operator=(Subcmd&&) noexcept = default;

  explicit Subcmd(const StringRef name) noexcept : name(name) {}

  Subcmd& setDesc(StringRef desc) noexcept;
  Subcmd& setShort(StringRef shortName) noexcept;
  Subcmd& addOpt(Opt opt) noexcept;
  Subcmd& setArg(Arg arg) noexcept;
  Subcmd& setMainFn(Fn<int(std::span<const StringRef>)> mainFn) noexcept;
  [[nodiscard]] int noSuchArg(StringRef arg) const;

private:
  Subcmd& setGlobalOpts(const Vec<Opt>& globalOpts) noexcept;
  bool hasShort() const noexcept;
  String getUsage() const noexcept;
  void printHelp() const noexcept;
  void print(usize maxOffset) const noexcept;

  usize calcMaxShortSize() const noexcept;
  /// Calculate the maximum length of the left side of the helps to align the
  /// descriptions with 2 spaces.
  usize calcMaxOffset(usize maxShortSize) const noexcept;
};

class Command {
  StringRef name;
  StringRef desc;
  HashMap<StringRef, Subcmd> subcmds;
  Vec<Opt> globalOpts;
  Vec<Opt> localOpts;

public:
  Command() noexcept = delete;
  ~Command() noexcept = default;
  Command(const Command&) noexcept = default;
  Command(Command&&) noexcept = default;
  Command& operator=(const Command&) noexcept = default;
  Command& operator=(Command&&) noexcept = default;

  explicit Command(const StringRef name) noexcept : name(name) {}

  Command& setDesc(StringRef desc) noexcept;
  Command& addSubcmd(Subcmd subcmd) noexcept;
  Command& addOpt(Opt opt) noexcept;
  bool hasSubcmd(StringRef subcmd) const noexcept;

  [[nodiscard]] int noSuchArg(StringRef arg) const;
  [[nodiscard]] int
  exec(StringRef subcmd, std::span<const StringRef> args) const;
  void printSubcmdHelp(StringRef subcmd) const noexcept;
  [[nodiscard]] int printHelp(std::span<const StringRef> args) const noexcept;

private:
  /// Print help message for poac itself.
  void printCmdHelp() const noexcept;
  usize calcMaxShortSize() const noexcept;
  usize calcMaxOffset(usize maxShortSize) const noexcept;
};
