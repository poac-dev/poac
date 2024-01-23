#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd lintCmd;

int lintMain(std::span<const StringRef> args);
