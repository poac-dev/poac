#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef initDesc =
    "Create a new poac package in an existing directory";

int initMain(Vec<String>);
void initHelp() noexcept;
