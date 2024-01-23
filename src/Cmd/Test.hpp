#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd testCmd;

int testMain(std::span<const StringRef> args);
