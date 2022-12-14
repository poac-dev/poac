#pragma once

// std
#include <utility>

// external
#include <ninja/build.h> // BuildConfig // NOLINT(build/include_order)
#include <ninja/build_log.h> // BuildLog, BuildLogUser // NOLINT(build/include_order)
#include <ninja/deps_log.h> // DepsLog // NOLINT(build/include_order)
#include <ninja/disk_interface.h> // RealDiskInterface // NOLINT(build/include_order)
#include <ninja/graph.h> // Node // NOLINT(build/include_order)
#include <ninja/metrics.h> // GetTimeMillis // NOLINT(build/include_order)
#include <ninja/state.h> // State // NOLINT(build/include_order)
#include <ninja/string_piece.h> // StringPiece // NOLINT(build/include_order)
#include <ninja/timestamp.h> // TimeStamp // NOLINT(build/include_order)
#include <spdlog/spdlog.h> // spdlog::error // NOLINT(build/include_order)

// internal
#include "poac/poac.hpp"

namespace poac::core::builder::data {

struct NinjaMain : public BuildLogUser {
  NinjaMain(const BuildConfig& config, Path build_dir)
      : config(config), build_dir(std::move(build_dir)) {}

  /// Build configuration set from flags (e.g. parallelism).
  const BuildConfig& config;

  /// Loaded state (rules, nodes).
  State state;

  /// Functions for accessing the disk.
  RealDiskInterface disk_interface;

  /// The build directory, used for storing the build log etc.
  Path build_dir;

  BuildLog build_log;
  DepsLog deps_log;

  i64 start_time_millis = GetTimeMillis();

  [[nodiscard]] Fn IsPathDead(StringPiece s) const->bool override {
    Node* n = state.LookupNode(s);
    if (n && n->in_edge()) {
      return false;
    }
    // Just checking n isn't enough: If an old output is both in the build log
    // and in the deps log, it will have a Node object in state_.  (It will
    // also have an in edge if one of its inputs is another output that's in
    // the deps log, but having a deps edge product an output that's input to
    // another deps edge is rare, and the first recompaction will delete all
    // old outputs from the deps log, and then a second recompaction will
    // clear the build log, which seems good enough for this corner case.) Do
    // keep entries around for files which still exist on disk, for generators
    // that want to use this information.
    String err;
    const TimeStamp mtime = disk_interface.Stat(s.AsString(), &err);
    if (mtime == -1) {
      spdlog::error(err); // Log and ignore Stat() errors.
    }
    return mtime == 0;
  }
};

} // namespace poac::core::builder::data
