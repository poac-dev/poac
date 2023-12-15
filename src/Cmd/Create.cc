#include "Create.hpp"

// std
#include <algorithm> // transform
#include <fstream>
#include <iostream>
#include <string>

// external
#include <git2-cpp/git2.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)

// internal
#include "../Config.hpp"
#include "../Data/Manifest.hpp"
#include "../Util/Format.hpp"
#include "../Util/Log.hpp"
#include "../Util/ResultMacros.hpp"
#include "../Util/Validator.hpp"

namespace poac::cmd::create {

auto to_string(ProjectType kind) -> String {
  switch (kind) {
    case ProjectType::Bin:
      return "binary (application)";
    case ProjectType::Lib:
      return "library";
    default:
      __builtin_unreachable();
  }
}

static auto operator<<(std::ostream& os, ProjectType kind) -> std::ostream& {
  return (os << to_string(kind));
}

template <typename T>
auto opts_to_project_type(T&& opts) -> ProjectType {
  opts.bin.value(); // Just check if opts has a `.bin` member
  return opts.lib.value() ? ProjectType::Lib : ProjectType::Bin;
}

namespace files {
  auto poac_toml(StringRef project_name) -> String {
    return format(
        "[package]\n"
        "name = \"{}\"\n"
        "version = \"0.1.0\"\n"
        "authors = []\n"
        "edition = 2020\n",
        project_name
    );
  }

  static constexpr StringRef MAIN_CPP =
      "#include <iostream>\n\n"
      "int main(int argc, char** argv) {\n"
      "  std::cout << \"Hello, world!\" << std::endl;\n"
      "}\n";

  static auto include_hpp(StringRef project_name) -> String {
    String project_name_upper_cased{};
    std::transform(
        project_name.cbegin(), project_name.cend(),
        std::back_inserter(project_name_upper_cased), ::toupper
    );

    return format(
        "#ifndef {0}_HPP\n"
        "#define {0}_HPP\n\n"
        "namespace {1} {{\n}}\n\n"
        "#endif // !{0}_HPP\n",
        project_name_upper_cased, project_name
    );
  }
} // namespace files

static void write_to_file(std::ofstream& ofs, const String& fname, StringRef text) {
  ofs.open(fname);
  if (ofs.is_open()) {
    ofs << text;
  }
  ofs.close();
  ofs.clear();
}

static auto create_template_files(const ProjectType& type, const String& package_name)
    -> Map<Path, String> {
  switch (type) {
    case ProjectType::Bin:
      fs::create_directories(package_name / "src"_path);
      return {
          {".gitignore", format("/{}", config::POAC_OUT)},
          {data::manifest::NAME, files::poac_toml(package_name)},
          {"src"_path / "main.cpp", String(files::MAIN_CPP)}};
    case ProjectType::Lib:
      fs::create_directories(package_name / "include"_path / package_name);
      return {
          {".gitignore", format("/{}\npoac.lock", config::POAC_OUT)},
          {data::manifest::NAME, files::poac_toml(package_name)},
          {"include"_path / package_name / (package_name + ".hpp"),
           files::include_hpp(package_name)},
      };
    default:
      __builtin_unreachable();
  }
}

[[nodiscard]] static auto create(const Options& opts) -> Result<void> {
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

[[nodiscard]] auto exec(const Options& opts) -> Result<void> {
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
