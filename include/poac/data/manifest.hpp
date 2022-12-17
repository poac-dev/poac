#pragma once

// external
#include <toml.hpp>

// internal
#include "poac/util/rustify.hpp"

namespace poac::data::manifest {

// NOLINTNEXTLINE(bugprone-exception-escape)
struct PartialPackage {
  String name;
  String version;
  i32 edition;
  Vec<String> authors;
  String license;
  String repository;
  String description;
};

} // namespace poac::data::manifest

// NOLINTNEXTLINE(modernize-use-trailing-return-type)
TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
    poac::data::manifest::PartialPackage, name, version, edition, authors,
    license, repository, description
)

namespace poac::data::manifest {

inline const String NAME = "poac.toml"; // NOLINT(readability-identifier-naming)

inline Fn poac_toml_last_modified(const Path& base_dir)->fs::file_time_type {
  return fs::last_write_time(base_dir / NAME);
}

} // namespace poac::data::manifest
