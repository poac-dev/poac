#pragma once

#include "../Cli.hpp"
#include "../Rustify.hpp"

#include <span>

// Poac pragma: Cmd/Build.cc
extern const Subcmd buildCmd;
int buildImpl(String& outDir, bool isDebug, bool isParallel);

// Poac pragma: Cmd/Clean.cc
extern const Subcmd cleanCmd;

// Poac pragma: Cmd/Fmt.cc
extern const Subcmd fmtCmd;

// Poac pragma: Cmd/Help.cc
extern const Subcmd helpCmd;
int helpMain(std::span<const StringRef> args) noexcept;

// Poac pragma: Cmd/Init.cc
extern const Subcmd initCmd;

// Poac pragma: Cmd/Lint.cc
extern const Subcmd lintCmd;

// Poac pragma: Cmd/New.cc
extern const Subcmd newCmd;
String createPoacToml(StringRef projectName) noexcept;

// Poac pragma: Cmd/Run.cc
extern const Subcmd runCmd;

// Poac pragma: Cmd/Search.cc
extern const Subcmd searchCmd;

// Poac pragma: Cmd/Test.cc
extern const Subcmd testCmd;

// Poac pragma: Cmd/Tidy.cc
extern const Subcmd tidyCmd;

// Poac pragma: Cmd/Version.cc
extern const Subcmd versionCmd;
int versionMain(std::span<const StringRef> args) noexcept;
