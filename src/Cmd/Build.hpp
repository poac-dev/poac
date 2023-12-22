#pragma once

#include "../Rustify.hpp"

#include <span>

static inline constexpr StringRef buildDesc =
    "Compile a local package and all of its dependencies";

int buildImpl(const bool, String&);
int buildMain(std::span<const StringRef>);
void buildHelp() noexcept;
