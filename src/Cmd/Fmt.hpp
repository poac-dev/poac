#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd fmtCmd;

int fmtMain(std::span<const StringRef> args);
