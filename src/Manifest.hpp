#pragma once

#include "Rustify.hpp"
#include "Semver.hpp"

struct DepMetadata {
  String includes; // -Isomething
  String libs; // -Lsomething -lsomething
};

struct Profile {
  HashSet<String> cxxflags;
  bool lto = false;

  // Merges this profile with another profile. If a field in this profile is
  // set, it will not be overwritten by the other profile. Only default values
  // will be overwritten.
  void merge(const Profile& other);
};

const Path& getManifestPath();
const String& getPackageName();
u16 editionToYear(const StringRef);
const String& getPackageEdition();
const Version& getPackageVersion();
const Profile& getDebugProfile();
const Profile& getReleaseProfile();
const Vec<String>& getLintCpplintFilters();
Vec<DepMetadata> installDependencies();
