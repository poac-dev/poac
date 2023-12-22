#pragma once

#include "Rustify.hpp"

String getPackageName();
String getPackageEdition();
u16 editionToYear(StringRef);
Vec<String> getLintCpplintFilters();
Vec<Path> installGitDependencies();
