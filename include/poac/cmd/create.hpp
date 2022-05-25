#ifndef POAC_CMD_CREATE_HPP_
#define POAC_CMD_CREATE_HPP_

// std
#include <algorithm> // transform
#include <fstream>
#include <iostream>
#include <string>

// external
#include <structopt/app.hpp>

// internal
#include "poac/poac.hpp"

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

String
to_string(ProjectType kind);

inline std::ostream&
operator<<(std::ostream& os, ProjectType kind) {
  return (os << to_string(kind));
}

template <typename T>
ProjectType
opts_to_project_type(T&& opts) {
  opts.bin.value(); // Just check opts has a `.bin` member
  return opts.lib.value() ? ProjectType::Lib : ProjectType::Bin;
}

namespace files {
  inline String
  poac_toml(StringRef project_name) {
    return format(
        "[package]\n"
        "name = \"{}\"\n"
        "version = \"0.1.0\"\n"
        "authors = []\n"
        "edition = 2020\n",
        project_name
    );
  }

  inline const String main_cpp(
      "#include <iostream>\n\n"
      "int main(int argc, char** argv) {\n"
      "  std::cout << \"Hello, world!\" << std::endl;\n"
      "}\n"
  );

  inline String
  include_hpp(StringRef project_name) {
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

void
write_to_file(std::ofstream& ofs, const String& fname, StringRef text);

Map<Path, String>
create_template_files(const ProjectType& type, const String& package_name);

[[nodiscard]] Result<void>
create(const Options& opts);

[[nodiscard]] Result<void>
exec(const Options& opts);

} // namespace poac::cmd::create

STRUCTOPT(poac::cmd::create::Options, package_name, bin, lib);

#endif // POAC_CMD_CREATE_HPP_
