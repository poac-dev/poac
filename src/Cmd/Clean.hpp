#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef cleanDesc = "Remove the built directory";

int cleanMain(Vec<String>);
void cleanHelp();
