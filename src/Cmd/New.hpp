#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd newCmd;

String createPoacToml(StringRef projectName) noexcept;
int newMain(std::span<const StringRef> args);
