#pragma once

#include "Rustify.hpp"
#include "Semver.hpp"

String getPackageName();
u16 editionToYear(StringRef);
String getPackageEdition();
Version getPackageVersion();
Vec<String> getLintCpplintFilters();
Vec<Path> installGitDependencies();
