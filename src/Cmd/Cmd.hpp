#pragma once

#include "../Cli.hpp"
#include "../Rustify.hpp"

#include <span>

// Poac pragma: Cmd/Build.cc
extern const Subcmd BUILD_CMD;
int buildImpl(String& outDir, bool isDebug, bool isParallel);

// Poac pragma: Cmd/Clean.cc
extern const Subcmd CLEAN_CMD; // TODO: should be CLEAN_CMD case

// Poac pragma: Cmd/Fmt.cc
extern const Subcmd FMT_CMD;

// Poac pragma: Cmd/Help.cc
extern const Subcmd HELP_CMD;

// Poac pragma: Cmd/Init.cc
extern const Subcmd INIT_CMD;

// Poac pragma: Cmd/Lint.cc
extern const Subcmd LINT_CMD;

// Poac pragma: Cmd/New.cc
extern const Subcmd NEW_CMD;
String createPoacToml(StringRef projectName) noexcept;

// Poac pragma: Cmd/Run.cc
extern const Subcmd RUN_CMD;

// Poac pragma: Cmd/Search.cc
extern const Subcmd SEARCH_CMD;

// Poac pragma: Cmd/Test.cc
extern const Subcmd TEST_CMD;

// Poac pragma: Cmd/Tidy.cc
extern const Subcmd TIDY_CMD;

// Poac pragma: Cmd/Version.cc
extern const Subcmd VERSION_CMD;
int versionMain(std::span<const StringRef> args) noexcept;
