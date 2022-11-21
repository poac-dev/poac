// external
#include <git2-cpp/git2.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "poac/cmd/create.hpp"
#include "poac/data/manifest.hpp"
#include "poac/util/validator.hpp"

namespace poac::cmd::create {

String
to_string(ProjectType kind) {
  switch (kind) {
    case ProjectType::Bin:
      return "binary (application)";
    case ProjectType::Lib:
      return "library";
    default:
      unreachable();
  }
}

void
write_to_file(std::ofstream& ofs, const String& fname, StringRef text) {
  ofs.open(fname);
  if (ofs.is_open()) {
    ofs << text;
  }
  ofs.close();
  ofs.clear();
}

Map<Path, String>
create_template_files(const ProjectType& type, const String& package_name) {
  switch (type) {
    case ProjectType::Bin:
      fs::create_directories(package_name / "src"_path);
      return {
          {".gitignore", "/poac-out"},
          {data::manifest::name, files::poac_toml(package_name)},
          {"src"_path / "main.cpp", String(files::main_cpp)}};
    case ProjectType::Lib:
      fs::create_directories(package_name / "include"_path / package_name);
      return {
          {".gitignore", "/poac-out\npoac.lock"},
          {data::manifest::name, files::poac_toml(package_name)},
          {"include"_path / package_name / (package_name + ".hpp"),
           files::include_hpp(package_name)},
      };
    default:
      unreachable();
  }
}

[[nodiscard]] Result<void>
create(const Options& opts) {
  std::ofstream ofs;
  const ProjectType type = opts_to_project_type(opts);
  for (auto&& [name, text] : create_template_files(type, opts.package_name)) {
    const String& file_path = (opts.package_name / name).string();
    spdlog::trace("Creating {}", file_path);
    write_to_file(ofs, file_path, text);
  }

  spdlog::trace("Initializing git repository at {}", opts.package_name);
  git2::repository().init(opts.package_name);

  log::status("Created", "{} `{}` package", to_string(type), opts.package_name);
  return Ok();
}

[[nodiscard]] Result<void>
exec(const Options& opts) {
  if (opts.bin.value() && opts.lib.value()) {
    return Err<PassingBothBinAndLib>();
  }

  namespace validator = util::validator;

  spdlog::trace("Validating the `{}` directory exists", opts.package_name);
  Try(validator::can_create_directory(opts.package_name).map_err(to_anyhow));

  spdlog::trace("Validating the package name `{}`", opts.package_name);
  Try(validator::valid_package_name(opts.package_name).map_err(to_anyhow));

  return create(opts);
}

} // namespace poac::cmd::create
