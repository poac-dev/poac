#ifndef POAC_DATA_MANIFEST_HPP_
#define POAC_DATA_MANIFEST_HPP_

// external
#include <toml.hpp>

// internal
#include "poac/poac.hh"

namespace poac::data::manifest {

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

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
    poac::data::manifest::PartialPackage, name, version, edition, authors,
    license, repository, description
)

namespace poac::data::manifest {

inline const String name = "poac.toml";

inline fs::file_time_type
poac_toml_last_modified(const Path& base_dir) {
  return fs::last_write_time(base_dir / name);
}

} // namespace poac::data::manifest

#endif // POAC_DATA_MANIFEST_HPP_
