#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd tidyCmd;

int tidyMain(std::span<const StringRef> args);
