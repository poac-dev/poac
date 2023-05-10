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
#include "poac/util/registry/conan/v1/manifest.hpp"

namespace poac::core::builder::manifest {

Fn rebuild(data::NinjaMain& ninja_main, Status& status, String& err)->bool {
  Node* node = ninja_main.state.LookupNode(
      (ninja_main.build_dir / MANIFEST_FILE_NAME).string()
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

Fn gather_includes(const resolver::ResolvedDeps& resolved_deps)->Vec<String> {
  Vec<String> includes;
  for (Let & [ package, inner_deps ] : resolved_deps) {
    static_cast<void>(inner_deps);

    const Path include_path = resolver::get_extracted_path(package) / "include";
    if (fs::exists(include_path) && fs::is_directory(include_path)) {
      includes.emplace_back(format("-I{}", include_path.string()));
    }
  }
  return includes;
}

Fn get_cfg_profile(const toml::value& poac_manifest)->Vec<toml::table> {
  Let target =
      toml::find_or<toml::table>(poac_manifest, "target", toml::table{});
  Vec<toml::table> profiles;
  for (Let & [ key, val ] : target) {
    if (key.find("cfg(") != None) {
      if (util::cfg::parse(key).match()) {
        Let profile = toml::find_or<toml::table>(val, "profile", toml::table{});
        profiles.emplace_back(profile);
      }
    }
  }
  return profiles;
}

Fn gather_flags(
    const toml::value& poac_manifest, const String& name,
    const Option<String>& prefix
)
    ->Vec<String> {
  auto f = toml::find_or<Vec<String>>(
      poac_manifest, "target", "profile", name, Vec<String>{}
  );
  if (prefix.has_value()) {
    std::transform(f.begin(), f.end(), f.begin(), [p = prefix.value()](Let& s) {
      return p + s;
    });
  }
  return f;
}

[[nodiscard]] Fn construct(
    const Path& build_dir, const toml::value& poac_manifest,
    const resolver::ResolvedDeps& resolved_deps
)
    ->Result<String> {
  syntax::Writer writer{std::ostringstream()};
  for (const StringRef header : MANIFEST_HEADERS) {
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
      format(
          "{} $in -o $out $OPTIONS $DEFINES $INCLUDES $LIBDIRS $LIBRARIES",
          command
      ),
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

  const Vec<String> options = gather_flags(poac_manifest, "options");
  Vec<String> includes = gather_includes(resolved_deps);
  Vec<String> defines = gather_flags(poac_manifest, "definitions", "-D");
  Vec<String> libraries = gather_flags(poac_manifest, "libraries", "-l");
  Vec<String> libdirs;

  auto conan_manifest =
      Try(poac::util::registry::conan::v1::manifest::gather_conan_deps());
  append(includes, conan_manifest.includes);
  append(defines, conan_manifest.defines);
  append(libraries, conan_manifest.libraries);
  append(libdirs, conan_manifest.libdirs);

  writer.build(
      {output_file.string()}, "compile",
      syntax::BuildSet{
          .inputs = std::vector{source_file.string()},
          .variables =
              syntax::Variables{
                  {"PACKAGE_NAME", name},
                  {"PACKAGE_VERSION", version},
                  {"PACKAGE_PATH", format("({})", config::cwd)},
                  {"OPTIONS", boost::algorithm::join(options, " ")},
                  {"DEFINES", boost::algorithm::join(defines, " ")},
                  {"INCLUDES", boost::algorithm::join(includes, " ")},
                  {"LIBDIRS", boost::algorithm::join(libdirs, " ")},
                  {"LIBRARIES", boost::algorithm::join(libraries, " ")},
              },
      }
  );
  writer.newline();

  writer.defalt({output_file.string()});
  return Ok(writer.get_value());
}

[[nodiscard]] Fn create(
    const Path& build_dir, const toml::value& poac_manifest,
    const resolver::ResolvedDeps& resolved_deps
)
    ->Result<void> {
  // TODO(ken-matsui): `build.ninja` will be constructed from `poac.toml`,
  //   so if `poac.toml` has no change,
  //   then `build.ninja` is not needed to be updated.
  //        if (is_outdated(build_dir)) {
  std::ofstream ofs(build_dir / MANIFEST_FILE_NAME, std::ios::out);
  ofs << Try(construct(build_dir, poac_manifest, resolved_deps));
  //        }
  return Ok();
}

} // namespace poac::core::builder::manifest
