#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd buildCmd;

int buildImpl(String& outDir, bool isDebug, bool isParallel);
int buildMain(std::span<const StringRef> args);
