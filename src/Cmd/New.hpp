#pragma once

#include "../Cli.hpp"
#include "../Rustify.hpp"

extern const Subcmd NEW_CMD;
String createPoacToml(StringRef projectName) noexcept;
