#pragma once

#include "Rustify.hpp"

String getPackageName();
u16 editionToYear(StringRef);
String getPackageEdition();
String getPackageVersion();
Vec<String> getLintCpplintFilters();
Vec<Path> installGitDependencies();
