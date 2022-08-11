// std
#include <fstream>
#include <string>

// external
#include <boost/algorithm/string.hpp> // boost::algorithm::join
#include <ninja/build.h> // Builder // NOLINT(build/include_order)
#include <ninja/graph.h> // Node // NOLINT(build/include_order)

// internal
#include "poac/core/builder/compiler.hpp"
#include "poac/core/builder/manifest.hpp"
#include "poac/core/builder/syntax.hpp"
#include "poac/core/resolver.hpp" // get_extracted_path
#include "poac/util/cfg.hpp"

namespace poac::core::builder::manifest {

bool
rebuild(data::NinjaMain& ninja_main, Status& status, String& err) {
  Node* node = ninja_main.state.LookupNode(
      (ninja_main.build_dir / manifest_file_name).string()
  );
  if (!node) {
    return false;
  }

  Builder builder(
      &ninja_main.state, ninja_main.config, &ninja_main.build_log,
      &ninja_main.deps_log, &ninja_main.disk_interface, &status,
      ninja_main.start_time_millis
  );
  if (!builder.AddTarget(node, &err)) {
    return false;
  }
  if (builder.AlreadyUpToDate()) {
    return false; // Not an error, but we didn't rebuild.
  }
  if (!builder.Build(&err)) {
    return false;
  }

  // The manifest was only rebuilt if it is now dirty (it may have been cleaned
  // by a restat).
  if (!node->dirty()) {
    // Reset the state to prevent problems like
    // https://github.com/ninja-build/ninja/issues/874
    ninja_main.state.Reset();
    return false;
  }
  return true;
}

Vec<String>
gather_includes(const resolver::ResolvedDeps& resolved_deps) {
  Vec<String> includes;
  for (const auto& [package, inner_deps] : resolved_deps) {
    static_cast<void>(inner_deps);

    const Path include_path = resolver::get_extracted_path(package) / "include";
    if (fs::exists(include_path) && fs::is_directory(include_path)) {
      includes.emplace_back(format("-I{}", include_path.string()));
    }
  }
  return includes;
}

Vec<toml::table>
get_cfg_profile(const toml::value& poac_manifest) {
  const auto target =
      toml::find_or<toml::table>(poac_manifest, "target", toml::table{});
  Vec<toml::table> profiles;
  for (const auto& [key, val] : target) {
    if (key.find("cfg(") != None) {
      if (util::cfg::parse(key).match()) {
        const auto profile =
            toml::find_or<toml::table>(val, "profile", toml::table{});
        profiles.emplace_back(profile);
      }
    }
  }
  return profiles;
}

Vec<String>
gather_flags(
    const toml::value& poac_manifest, const String& name,
    const Option<String>& prefix
) {
  auto f = toml::find_or<Vec<String>>(
      poac_manifest, "target", "profile", name, Vec<String>{}
  );
  if (prefix.has_value()) {
    std::transform(
        f.begin(), f.end(), f.begin(),
        [p = prefix.value()](const auto& s) { return p + s; }
    );
  }
  return f;
}

[[nodiscard]] Result<String>
construct(
    const Path& build_dir, const toml::value& poac_manifest,
    const resolver::ResolvedDeps& resolved_deps
) {
  syntax::Writer writer{std::ostringstream()};
  for (StringRef header : manifest_headers) {
    writer.comment(String(header));
  }
  writer.newline();

  const String name = toml::find<String>(poac_manifest, "package", "name");
  const String version =
      toml::find<String>(poac_manifest, "package", "version");
  const i64 edition = toml::find<i64>(poac_manifest, "package", "edition");
  const String command = Try(compiler::cxx::get_command(edition, false));

  writer.rule(
      "compile",
      format("{} $OPTIONS $DEFINES $INCLUDES $LIBRARIES $in -o $out", command),
      syntax::RuleSet{
          .description = "$PACKAGE_NAME v$PACKAGE_VERSION $PACKAGE_PATH",
      }
  );
  writer.newline();

  const Path source_file = "src"_path / "main.cpp";
  Path output_file;
  if (source_file == "src"_path / "main.cpp") {
    // When building src/main.cpp, the output executable should be stored at
    // poac-out/debug/name
    output_file = build_dir / name;
  } else {
    output_file = (build_dir / source_file).string() + ".o";
    fs::create_directories(output_file.parent_path());
  }
  const Vec<String> includes = gather_includes(resolved_deps);

  const Vec<String> defines = gather_flags(poac_manifest, "definitions", "-D");
  const Vec<String> options = gather_flags(poac_manifest, "options");
  const Vec<String> libraries = gather_flags(poac_manifest, "libraries", "-l");

  writer.build(
      {output_file.string()}, "compile",
      syntax::BuildSet{
          .inputs = std::vector{source_file.string()},
          .variables =
              syntax::Variables{
                  {"PACKAGE_NAME", name},
                  {"PACKAGE_VERSION", version},
                  {"PACKAGE_PATH", format("({})", config::path::cur_dir)},
                  {"OPTIONS", boost::algorithm::join(options, " ")},
                  {"DEFINES", boost::algorithm::join(defines, " ")},
                  {"INCLUDES", boost::algorithm::join(includes, " ")},
                  {"LIBRARIES", boost::algorithm::join(libraries, " ")},
              },
      }
  );
  writer.newline();

  writer.default_({output_file.string()});
  return Ok(writer.get_value());
}

[[nodiscard]] Result<void>
create(
    const Path& build_dir, const toml::value& poac_manifest,
    const resolver::ResolvedDeps& resolved_deps
) {
  // TODO(ken-matsui): `ninja.build` will be constructed from `poac.toml`,
  //   so if `poac.toml` has no change,
  //   then `ninja.build` is not needed to be updated.
  //        if (is_outdated(build_dir)) {
  std::ofstream ofs(build_dir / manifest_file_name, std::ios::out);
  ofs << Try(construct(build_dir, poac_manifest, resolved_deps));
  //        }
  return Ok();
}

} // namespace poac::core::builder::manifest
