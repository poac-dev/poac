#pragma once

#include "../Cli.hpp"
#include "../Rustify.hpp"

extern const Subcmd BUILD_CMD;
int buildImpl(String& outDir, bool isDebug);
