#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd initCmd;

int initMain(std::span<const StringRef> args);
