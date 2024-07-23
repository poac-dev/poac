#pragma once

#include "../Cli.hpp"

#include <string>

extern const Subcmd BUILD_CMD;
int buildImpl(std::string& outDir, bool isDebug);
