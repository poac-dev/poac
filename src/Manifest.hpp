#pragma once

#include "Rustify.hpp"
#include "Semver.hpp"

struct DepMetadata {
  String includes; // -Isomething
  String libs; // -Lsomething -lsomething
};

String getPackageName();
u16 editionToYear(StringRef);
String getPackageEdition();
Version getPackageVersion();
Vec<String> getLintCpplintFilters();
Vec<DepMetadata> installDependencies();
