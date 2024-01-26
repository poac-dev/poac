#pragma once

#include "../Cli.hpp"
#include "../Rustify.hpp"

#include <span>

// Poac pragma: defined in Cmd/Build.cc
extern const Subcmd BUILD_CMD;
int buildImpl(String& outDir, bool isDebug, bool isParallel);

// Poac pragma: defined in Cmd/Clean.cc
extern const Subcmd CLEAN_CMD;

// Poac pragma: defined in Cmd/Fmt.cc
extern const Subcmd FMT_CMD;

// Poac pragma: defined in Cmd/Help.cc
extern const Subcmd HELP_CMD;

// Poac pragma: defined in Cmd/Init.cc
extern const Subcmd INIT_CMD;

// Poac pragma: defined in Cmd/Lint.cc
extern const Subcmd LINT_CMD;

// Poac pragma: defined in Cmd/New.cc
extern const Subcmd NEW_CMD;
String createPoacToml(StringRef projectName) noexcept;

// Poac pragma: defined in Cmd/Run.cc
extern const Subcmd RUN_CMD;

// Poac pragma: defined in Cmd/Search.cc
extern const Subcmd SEARCH_CMD;

// Poac pragma: defined in Cmd/Test.cc
extern const Subcmd TEST_CMD;

// Poac pragma: defined in Cmd/Tidy.cc
extern const Subcmd TIDY_CMD;

// Poac pragma: defined in Cmd/Version.cc
extern const Subcmd VERSION_CMD;
int versionMain(std::span<const StringRef> args) noexcept;
