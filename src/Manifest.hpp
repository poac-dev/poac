#pragma once

#include "Rustify.hpp"
#include "Semver.hpp"

struct DepMetadata {
  String includes; // -Isomething
  String libs; // -Lsomething -lsomething
};

struct Profile {
  bool lto = false;

  // Merges this profile with another profile. If a field in this profile is
  // set, it will not be overwritten by the other profile. Only default values
  // will be overwritten.
  void merge(const Profile& other);
};

String getPackageName();
u16 editionToYear(StringRef);
String getPackageEdition();
Version getPackageVersion();
Profile getDebugProfile();
Profile getReleaseProfile();
Vec<String> getLintCpplintFilters();
Vec<DepMetadata> installDependencies();
