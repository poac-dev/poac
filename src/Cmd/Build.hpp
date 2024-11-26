#pragma once

#include "../Cli.hpp"

#include <string>

enum class BuildSystem {
    Makefile,
    Xmake
};

static BuildSystem getDefaultBuildSystem() {
#ifdef _WIN32
    return BuildSystem::Xmake;
#else
    return BuildSystem::Makefile;
#endif
}

extern const Subcmd BUILD_CMD;
int buildImpl(std::string& outDir, bool isDebug, BuildSystem system);
