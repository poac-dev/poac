#pragma once

#include "../Cli.hpp"
#include "../Rustify.hpp"

#include <string>

extern const Subcmd NEW_CMD;
std::string createPoacToml(StringRef projectName) noexcept;
