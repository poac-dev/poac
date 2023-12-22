#pragma once

#include "Rustify.hpp"

String getPackageName();
String getPackageEdition();
u16 editionToYear(StringRef);
String getPackageVersion();
Vec<String> getLintCpplintFilters();
Vec<Path> installGitDependencies();
