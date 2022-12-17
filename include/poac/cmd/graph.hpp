#pragma once

// std
#include <utility>

// external
#include <boost/graph/adjacency_list.hpp>
#include <structopt/app.hpp>

// internal
#include "poac/core/resolver.hpp"
#include "poac/util/format.hpp"
#include "poac/util/log.hpp"
#include "poac/util/result.hpp"
#include "poac/util/rustify.hpp"

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

// NOLINTNEXTLINE(bugprone-exception-escape)
struct Vertex {
  String name;
  String version;
};
using Graph =
    boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex>;

Fn create_resolved_deps()->Result<core::resolver::ResolvedDeps>;

Fn create_graph()->Result<std::pair<Graph, Vec<String>>>;

[[nodiscard]] Fn dot_file_output(const Path& output_path)->Result<void>;

[[nodiscard]] Fn png_file_output(const Path& output_path)->Result<void>;

[[nodiscard]] Fn file_output(const Path& output_path)->Result<void>;

[[nodiscard]] Fn console_output()->Result<void>;

[[nodiscard]] Fn exec(const Options& opts)->Result<void>;

} // namespace poac::cmd::graph

STRUCTOPT(poac::cmd::graph::Options, output_file);
