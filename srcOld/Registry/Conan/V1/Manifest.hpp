module;

// std
#include <fstream>
#include <string>

// external
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

export module poac.util.registry.conan.v1.manifest;

import poac.util.result;
import poac.util.rustify;
import poac.config;
import poac.util.format;

export namespace poac::util::registry::conan::v1::manifest {

auto gather_conan_conf(
    const boost::property_tree::ptree& pt, const std::string& field,
    const std::string& prefix
) -> Vec<String> {
  Vec<String> lines;

  for (const auto& s : pt.get_child(field)) {
    lines.push_back(format("{}{}", prefix, s.second.data()));
  }

  return lines;
}

inline auto gather_conan_defines(const boost::property_tree::ptree& pt)
    -> Vec<String> {
  return gather_conan_conf(pt, "defines", "-D");
}

inline auto gather_conan_includes(const boost::property_tree::ptree& pt)
    -> Vec<String> {
  return gather_conan_conf(pt, "include_paths", "-I");
}

inline auto gather_conan_libdirs(const boost::property_tree::ptree& pt)
    -> Vec<String> {
  return gather_conan_conf(pt, "lib_paths", "-L");
}

inline auto gather_conan_libraries(const boost::property_tree::ptree& pt)
    -> Vec<String> {
  return gather_conan_conf(pt, "libs", "-l");
}

struct ConanManifest {
  Vec<String> defines;
  Vec<String> includes;
  Vec<String> libdirs;
  Vec<String> libraries;
};

auto gather_conan_deps() -> Result<ConanManifest> {
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
      .libraries = gather_conan_libraries(pt)
  });
}

} // namespace poac::util::registry::conan::v1::manifest
