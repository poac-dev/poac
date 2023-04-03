#pragma once

// std
#include <string>

// external
#include <boost/property_tree/ptree.hpp>

// internal
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::util::registry::conan::v1::manifest {

Vec<String> gather_conan_conf(
    const boost::property_tree::ptree& pt, const std::string& field,
    const std::string& prefix
);

struct ConanManifest {
  Vec<String> defines;
  Vec<String> includes;
  Vec<String> libdirs;
  Vec<String> libraries;
};

Result<ConanManifest> gather_conan_deps();

} // namespace poac::util::registry::conan::v1::manifest
