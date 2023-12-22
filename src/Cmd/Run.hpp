#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef runDesc = "Build and execute src/main.cc";

int runMain(std::span<const StringRef>);
void runHelp() noexcept;
