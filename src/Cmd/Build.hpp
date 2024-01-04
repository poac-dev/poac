#pragma once

#include "../Rustify.hpp"

#include <span>

// NOLINTNEXTLINE(readability-identifier-naming)
static inline constexpr StringRef buildDesc =
    "Compile a local package and all of its dependencies";

int buildImpl(String&, const bool, const bool);
int buildMain(std::span<const StringRef>);
void buildHelp() noexcept;
