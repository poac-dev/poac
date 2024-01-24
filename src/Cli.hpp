#pragma once

#include "Rustify.hpp"

#include <cstdlib>
#include <span>
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

class Opt;
class Arg;
class Subcmd;
class Command;

void printHeader(StringRef header) noexcept;
void printUsage(StringRef cmd, StringRef usage) noexcept;
void printCommand(StringRef name, const Subcmd& cmd, usize maxOffset) noexcept;
void printGlobalOpts(usize maxShortSize, usize maxOffset) noexcept;

class Opt {
public:
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
public:
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

// TODO: Delete this.
inline constinit const Arr<Opt, 4> GLOBAL_OPTS{
  Opt{ "--verbose" }.setShort("-v").setDesc("Use verbose output"),
  Opt{ "--quiet" }.setShort("-q").setDesc("Do not print poac log messages"),
  Opt{ "--color" }
      .setDesc("Coloring: auto, always, never")
      .setPlaceholder("<WHEN>"),
  Opt{ "--help" }.setShort("-h").setDesc("Print help"),
};
