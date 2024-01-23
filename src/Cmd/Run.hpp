#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd runCmd;

int runMain(std::span<const StringRef> args);
