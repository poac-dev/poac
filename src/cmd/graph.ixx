module;

// std
#include <algorithm>
#include <fstream>
#include <unordered_map>
#include <utility>

// external
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <spdlog/spdlog.h> // NOLINT(build/include_order)
#include <structopt/app.hpp>
#include <toml.hpp>

// internal
#include "../util/result-macros.hpp"

export module poac.cmd.graph;

// internal
import poac.cmd.build;
import poac.core.resolver.types; // ResolvedDeps
import poac.core.resolver; // install_deps
import poac.data.manifest;
import poac.util.format;
import poac.util.log;
import poac.util.result;
import poac.util.rustify;
import poac.util.shell;

namespace poac::cmd::graph {

export struct Options : structopt::sub_command {
  // Perform only checks
  Option<Path> output_file = None;
};

using ExtError = Error<"The extension of the output file must be .dot or .png">;
using GraphvizNotFound = Error<
    "`graph` command requires `graphviz`; try:\n"
    "  apt/brew install graphviz\n"
    "Or consider outputting this as `.dot`">;

// NOLINTNEXTLINE(bugprone-exception-escape)
struct Vertex {
  String name;
  String version;
};
using Graph =
    boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex>;

auto create_resolved_deps() -> Result<core::resolver::ResolvedDeps> {
  spdlog::trace("Parsing the manifest file ...");
  // TODO(ken-matsui): parse as a static type rather than toml::value
  const toml::value manifest = toml::parse(data::manifest::NAME);

  return core::resolver::install_deps(manifest).with_context([] {
    return Err<build::FailedToInstallDeps>().get();
  });
}

auto create_graph() -> Result<std::pair<Graph, Vec<String>>> {
  const core::resolver::ResolvedDeps resolved_deps =
      Try(create_resolved_deps());
  Graph g;

  // Add vertex
  Vec<Graph::vertex_descriptor> desc;
  for (const auto& dep : resolved_deps | boost::adaptors::indexed()) {
    desc.push_back(boost::add_vertex(g));

    const i64 index = dep.index();
    const core::resolver::resolve::Package package = dep.value().first;

    g[index].name = package.name;
    g[index].version = package.dep_info.version_rq;
  }

  // Add edge
  for (const auto& dep : resolved_deps | boost::adaptors::indexed()) {
    const i64 index = dep.index();
    const auto deps = dep.value().second;

    if (deps.has_value()) {
      for (const auto& [name, version] : deps.value()) {
        auto first = resolved_deps.cbegin();
        auto last = resolved_deps.cend();

        const auto result = std::find_if(first, last, [&n = name](auto d) {
          // dependency should be resolved as only one package.
          return d.first.name == n;
        });
        if (result != last) {
          boost::add_edge(desc[index], desc[std::distance(first, result)], g);
        }
      }
    }
  }

  Vec<String> names;
  for (const auto& [package, deps] : resolved_deps) {
    static_cast<void>(deps);
    names.push_back(package.name + ": " + package.dep_info.version_rq);
  }
  return Ok(std::make_pair(g, names));
}

[[nodiscard]] auto dot_file_output(const Path& output_path) -> Result<void> {
  const auto [g, names] = Try(create_graph());
  std::ofstream file(output_path);
  boost::write_graphviz(file, g, boost::make_label_writer(names.data()));
  return Ok();
}

[[nodiscard]] auto png_file_output(const Path& output_path) -> Result<void> {
  if (util::shell::has_command("dot")) {
    const auto [g, names] = Try(create_graph());

    const String file_dot = output_path.stem().string() + ".dot";
    std::ofstream file(file_dot);
    boost::write_graphviz(file, g, boost::make_label_writer(names.data()));

    std::ignore = util::shell::Cmd(
                      "dot -Tpng " + file_dot + " -o " + output_path.string()
    )
                      .exec();
    std::filesystem::remove(file_dot);
    return Ok();
  } else {
    return Err<GraphvizNotFound>();
  }
}

[[nodiscard]] auto file_output(const Path& output_path) -> Result<void> {
  if (output_path.extension() == ".png") {
    return png_file_output(output_path);
  } else if (output_path.extension() == ".dot") {
    return dot_file_output(output_path);
  } else {
    return Err<ExtError>();
  }
}

[[nodiscard]] auto console_output() -> Result<void> {
  const auto [g, names] = Try(create_graph());
  static_cast<void>(names); // error: unused variable
  for (auto [itr, end] = edges(g); itr != end; ++itr) {
    spdlog::info(
        "{} -> {}", boost::get(&Vertex::name, g)[source(*itr, g)],
        boost::get(&Vertex::name, g)[target(*itr, g)]
    );
  }
  return Ok();
}

export [[nodiscard]] auto exec(const Options& opts) -> Result<void> {
  if (opts.output_file.has_value()) {
    Try(file_output(opts.output_file.value()));
    log::status("Generated", opts.output_file.value());
    return Ok();
  } else {
    return console_output();
  }
}

} // namespace poac::cmd::graph

STRUCTOPT(poac::cmd::graph::Options, output_file);
