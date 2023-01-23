#pragma once

// std
#include <algorithm> // transform
#include <fstream>
#include <iostream>
#include <string>

// external
#include <structopt/app.hpp>

// internal
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

namespace poac::cmd::create {

struct Options : structopt::sub_command {
  /// Package name to create a new poac package
  String package_name;

  /// Use a binary (application) template [default]
  Option<bool> bin = false;
  /// Use a library template
  Option<bool> lib = false;
};

using PassingBothBinAndLib =
    Error<"cannot specify both lib and binary outputs">;

enum class ProjectType {
  Bin,
  Lib,
};

Fn to_string(ProjectType kind)->String;

inline Fn operator<<(std::ostream& os, ProjectType kind)->std::ostream& {
  return (os << to_string(kind));
}

template <typename T>
Fn opts_to_project_type(T&& opts)->ProjectType {
  opts.bin.value(); // Just check if opts has a `.bin` member
  return opts.lib.value() ? ProjectType::Lib : ProjectType::Bin;
}

namespace files {
  inline Fn poac_toml(StringRef project_name)->String {
    return format(
        "[package]\n"
        "name = \"{}\"\n"
        "version = \"0.1.0\"\n"
        "authors = []\n"
        "edition = 2020\n",
        project_name
    );
  }

  inline constexpr StringRef MAIN_CPP =
      "#include <iostream>\n\n"
      "int main(int argc, char** argv) {\n"
      "  std::cout << \"Hello, world!\" << std::endl;\n"
      "}\n";

  inline Fn include_hpp(StringRef project_name)->String {
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

void write_to_file(std::ofstream& ofs, const String& fname, StringRef text);

Fn create_template_files(const ProjectType& type, const String& package_name)
    ->Map<Path, String>;

[[nodiscard]] Fn create(const Options& opts)->Result<void>;

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::create

STRUCTOPT(poac::cmd::create::Options, package_name, bin, lib);
