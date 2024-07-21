#pragma once

#include "Rustify.hpp"
#include "Semver.hpp"

#include <compare>
#include <string>

struct DepMetadata {
  std::string includes; // -Isomething
  std::string libs; // -Lsomething -lsomething
};

struct Profile {
  HashSet<std::string> cxxflags;
  bool lto = false;

  // Merges this profile with another profile. If a field in this profile is
  // set, it will not be overwritten by the other profile. Only default values
  // will be overwritten.
  void merge(const Profile& other);
};

struct Edition {
  enum class Year : u16 {
    Cpp98 = 1998,
    Cpp03 = 2003,
    Cpp11 = 2011,
    Cpp14 = 2014,
    Cpp17 = 2017,
    Cpp20 = 2020,
    Cpp23 = 2023,
    Cpp26 = 2026,
  };
  using enum Year;

private:
  Year edition = Year::Cpp20;
  std::string str = "20";

public:
  Edition() = default;
  explicit Edition(const std::string& str);

  std::string getString() const noexcept;

  inline auto operator<=>(const Edition& otherEdition) const {
    return edition <=> otherEdition.edition;
  }
  inline auto operator<=>(const Year& otherYear) const {
    return edition <=> otherYear;
  }
};

const fs::path& getManifestPath();
Option<std::string> validatePackageName(StringRef name) noexcept;
const std::string& getPackageName();
const Edition& getPackageEdition();
const Version& getPackageVersion();
const Profile& getDebugProfile();
const Profile& getReleaseProfile();
const Vec<std::string>& getLintCpplintFilters();
Vec<DepMetadata> installDependencies();
