// std
#include <fstream>

// external
#include <boost/property_tree/json_parser.hpp>

// internal
#include "poac/config.hpp"
#include "poac/util/format.hpp"
#include "poac/util/registry/conan/v1/manifest.hpp"

namespace poac::util::registry::conan::v1::manifest {

Fn gather_conan_conf(
    const boost::property_tree::ptree& pt, const std::string& field,
    const std::string& prefix
)
    ->Vec<String> {
  Vec<String> lines;

  for (const auto& s : pt.get_child(field)) {
    lines.push_back(fmt::format("{}{}", prefix, s.second.data()));
  }

  return lines;
}

inline Fn gather_conan_defines(const boost::property_tree::ptree& pt)
    ->Vec<String> {
  return gather_conan_conf(pt, "defines", "-D");
}

inline Fn gather_conan_includes(const boost::property_tree::ptree& pt)
    ->Vec<String> {
  return gather_conan_conf(pt, "include_paths", "-I");
}

inline Fn gather_conan_libdirs(const boost::property_tree::ptree& pt)
    ->Vec<String> {
  return gather_conan_conf(pt, "lib_paths", "-L");
}

inline Fn gather_conan_libraries(const boost::property_tree::ptree& pt)
    ->Vec<String> {
  return gather_conan_conf(pt, "libs", "-l");
}

Fn gather_conan_deps()->Result<ConanManifest> {
  if (!fs::exists(config::conan_deps_file)) {
    return Ok(ConanManifest{});
  }

  std::ifstream ifs(config::conan_deps_file);
  boost::property_tree::ptree pt;
  read_json(ifs, pt);

  return Ok(ConanManifest{
      .defines = gather_conan_defines(pt),
      .includes = gather_conan_includes(pt),
      .libdirs = gather_conan_libdirs(pt),
      .libraries = gather_conan_libraries(pt)});
}

} // namespace poac::util::registry::conan::v1::manifest
