#ifndef POAC_CMD_WIP_GRAPH_HPP_
#define POAC_CMD_WIP_GRAPH_HPP_

// std
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <future>
#include <iostream>
#include <iterator>
#include <optional>
#include <string>
#include <utility>
#include <vector>

// external
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/range/adaptor/indexed.hpp>
#include <boost/range/iterator_range_core.hpp>

// internal
#include <poac/core/except.hpp>
#include <poac/core/resolver/resolve.hh>
#include <poac/io/config.hpp>
#include <poac/io/term.hpp>
#include <poac/util/shell.hh>

namespace poac::opts::graph {

inline const clap::subcommand cli =
    clap::subcommand("graph")
        .about("Create a dependency graph")
        .arg(clap::opt("output", "Output file name")
                 .short_("o")
                 .value_name("NAME"));

struct Options {
  Option<std::filesystem::path> output_file;
};

struct Vertex {
  std::string name;
  std::string version;
};
using Graph =
    boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex>;

core::resolver::resolve::ResolvedDeps
create_resolved_deps(Option<io::config::Config>&& config) {
  namespace resolver = core::resolver::resolve;

  if (!config->dependencies) {
    throw core::except::error(
        core::except::msg::key_does_not_exist("dependencies")
    );
  }
  const auto deps_node = config->dependencies.value();

  // create resolved deps
  resolver::ResolvedDeps resolved_deps{};
  //        if (const auto locked_deps = core::resolver::lock::load()) {
  //            resolved_deps = locked_deps.value();
  //        } else { // poac.lock does not exist
  const resolver::NoDuplicateDeps deps = install::resolve_packages(deps_node);
  resolved_deps = resolver::resolve(deps);
  //        }
  return resolved_deps;
}

std::pair<Graph, Vec<std::string>>
create_graph(std::future<Option<io::config::Config>>&& config) {
  const auto lockfile = io::lockfile::load();

  const auto resolved_deps = create_resolved_deps(config.get());
  Graph g;

  // Add vertex
  Vec<Graph::vertex_descriptor> desc;
  for (const boost::range::index_value<
           const std::pair<
               const std::string, poac::io::lockfile::Lockfile::Package>&,
           i64>
           dep : lockfile->dependencies | boost::adaptors::indexed()) {
    desc.push_back(boost::add_vertex(g));
    g[dep.index()].name = dep.value().first;
    g[dep.index()].version = dep.value().second.version;
  }
  // Add edge
  for (const boost::range::index_value<
           const std::pair<std::string, poac::io::lockfile::Lockfile::Package>&,
           i64>
           dep : resolved_deps.duplicate_deps | boost::adaptors::indexed()) {
    if (!dep.value().second.dependencies.has_value()) {
      for (const auto& [name, version] :
           dep.value().second.dependencies.value()) {
        const auto result = std::find_if(
            resolved_deps.duplicate_deps.begin(),
            resolved_deps.duplicate_deps.end(),
            [&n = name, &v = version](auto d) {
              return d.first == n && d.second.version == v;
            }
        );
        if (result != resolved_deps.duplicate_deps.end()) {
          const auto index =
              std::distance(resolved_deps.duplicate_deps.begin(), result);
          boost::add_edge(desc[dep.index()], desc[index], g);
        }
      }
    }
  }

  // TODO(ken-matsui): Such as <algorithm> can do the same thing with the
  // following
  Vec<std::string> names;
  for (const auto& [name, package] : resolved_deps.duplicate_deps) {
    names.push_back(name + ": " + package.version);
  }
  return {g, names};
}

[[nodiscard]] Option<core::except::Error>
dot_file_output(
    std::future<Option<io::config::Config>>&& config, graph::Options&& opts
) {
  const auto [g, names] = create_graph(std::move(config));
  std::ofstream file(opts.output_file->string());
  boost::write_graphviz(file, g, boost::make_label_writer(&names[0]));
  io::term::status_done();
  return None;
}

[[nodiscard]] Option<core::except::Error>
png_file_output(
    std::future<Option<io::config::Config>>&& config, graph::Options&& opts
) {
  if (util::_shell::has_command("dot")) {
    const auto [g, names] = create_graph(std::move(config));

    const std::string file_dot = opts.output_file->stem().string() + ".dot";
    std::ofstream file(file_dot);
    boost::write_graphviz(file, g, boost::make_label_writer(&names[0]));

    util::shell("dot -Tpng " + file_dot + " -o " + opts.output_file->string())
        .exec();
    std::filesystem::remove(file_dot);
    io::term::status_done();
    return None;
  } else {
    return core::except::Error::General{
        "To output with .png you need to install the graphviz.\n"
        "Or please consider outputting in .dot format."};
  }
}

[[nodiscard]] Option<core::except::Error>
file_output(
    std::future<Option<io::config::Config>>&& config, graph::Options&& opts
) {
  if (opts.output_file->extension() == ".png") {
    return png_file_output(std::move(config), std::move(opts));
  } else if (opts.output_file->extension() == ".dot") {
    return dot_file_output(std::move(config), std::move(opts));
  } else {
    return core::except::Error::General{
        "The extension of the output file must be .dot or .png."};
  }
}

[[nodiscard]] Option<core::except::Error>
console_output(std::future<Option<io::config::Config>>&& config) {
  const auto [g, names] = create_graph(std::move(config));
  static_cast<void>(names); // error: unused variable
  for (auto [itr, end] = edges(g); itr != end; ++itr) {
    std::cout << boost::get(&Vertex::name, g)[source(*itr, g)] << " -> "
              << boost::get(&Vertex::name, g)[target(*itr, g)] << '\n';
  }
  return None;
}

[[nodiscard]] Option<core::except::Error>
graph(std::future<Option<io::config::Config>>&& config, graph::Options&& opts) {
  if (opts.output_file) {
    return file_output(std::move(config), std::move(opts));
  } else {
    return console_output(std::move(config));
  }
}

[[nodiscard]] Option<core::except::Error>
exec(
    std::future<Option<io::config::Config>>&& config, Vec<std::string>&& args
) {
  graph::Options opts{};
  opts.output_file = util::argparse::use_get(args, "-o", "--output");
  return graph::graph(std::move(config), std::move(opts));
}

} // namespace poac::opts::graph

#endif // POAC_CMD_WIP_GRAPH_HPP_
