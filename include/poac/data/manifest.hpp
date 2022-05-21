#ifndef POAC_DATA_MANIFEST_HPP_
#define POAC_DATA_MANIFEST_HPP_

// std
#include <vector>

// external
#include <toml.hpp>

// internal
#include <poac/poac.hpp>

namespace poac::data::manifest {

    struct PartialPackage {
        std::string name;
        std::string version;
        std::int32_t edition;
        std::vector<std::string> authors;
        std::string license;
        std::string repository;
        std::string description;
    };

}

TOML11_DEFINE_CONVERSION_NON_INTRUSIVE(
    poac::data::manifest::PartialPackage,
    name, version, edition, authors, license, repository, description
)

namespace poac::data::manifest {

inline const String name = "poac.toml";

inline fs::file_time_type
poac_toml_last_modified(const fs::path& base_dir) {
  return fs::last_write_time(base_dir / name);
}

} // namespace poac::data::manifest

#endif // POAC_DATA_MANIFEST_HPP_
