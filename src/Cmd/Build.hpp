#pragma once

#include "../Cli.hpp"

#include <string>

enum class BuildSystem {
    Makefile,
    Xmake
};

extern BuildSystem getDefaultBuildSystem();

extern const Subcmd BUILD_CMD;
int buildImpl(std::string& outDir, bool isDebug, BuildSystem system);
