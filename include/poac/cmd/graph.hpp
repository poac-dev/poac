#ifndef POAC_CMD_GRAPH_HPP_
#define POAC_CMD_GRAPH_HPP_

// std
#include <utility>

// external
#include <boost/graph/adjacency_list.hpp>
#include <structopt/app.hpp>

// internal
#include "poac/core/resolver.hpp"
#include "poac/poac.hpp"

namespace poac::cmd::graph {

struct Options : structopt::sub_command {
  // Perform only checks
  Option<Path> output_file = None;
};

using ExtError = Error<"The extension of the output file must be .dot or .png">;
using GraphvizNotFound = Error<
    "`graph` command requires `graphviz`; try installing it by:\n"
    "  apt/brew install graphviz\n"
    "Or consider outputting this as `.dot`">;
using FailedToInstallDeps = Error<"failed to install dependencies">;

struct Vertex {
  String name;
  String version;
};
using Graph =
    boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex>;

Result<core::resolver::ResolvedDeps> create_resolved_deps();

Result<std::pair<Graph, Vec<String>>> create_graph();

[[nodiscard]] Result<void> dot_file_output(const Path& output_path);

[[nodiscard]] Result<void> png_file_output(const Path& output_path);

[[nodiscard]] Result<void> file_output(const Path& output_path);

[[nodiscard]] Result<void> console_output();

[[nodiscard]] Result<void> exec(const Options& opts);

} // namespace poac::cmd::graph

STRUCTOPT(poac::cmd::graph::Options, output_file);

#endif // POAC_CMD_GRAPH_HPP_
