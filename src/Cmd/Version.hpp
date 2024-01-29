#pragma once

#include "../Cli.hpp"
#include "../Rustify.hpp"

#include <span>

extern const Subcmd VERSION_CMD;
int versionMain(std::span<const StringRef> args) noexcept;
