#pragma once

#include "../Rustify.hpp"

static inline constexpr StringRef buildDesc =
    "Compile a local package and all of its dependencies";

int buildImpl(const bool, String&);
int buildCmd(Vec<String>);
void buildHelp();
