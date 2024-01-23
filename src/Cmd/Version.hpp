#pragma once

#include "../Rustify.hpp"
#include "Global.hpp"

#include <span>

extern const Subcmd versionCmd;

int versionMain(std::span<const StringRef> args) noexcept;
