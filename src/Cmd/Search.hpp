#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd searchCmd;

int searchMain(std::span<const StringRef> args);
