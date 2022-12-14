#pragma once

// std
#include <ostream>
#include <string>

// external
#include <ninja/state.h> // NOLINT(build/include_order)
#include <toml.hpp>

// internal
#include "poac/core/builder/data.hpp"
#include "poac/core/resolver/types.hpp" // ResolvedDeps
#include "poac/poac.hpp"

namespace poac::core::builder::build {

using GeneralError =
    Error<"internal build system has been stopped with an error:\n{}", String>;

enum class Mode {
  debug,
  release,
};

Fn to_string(Mode mode)->String;

Fn operator<<(std::ostream& os, Mode mode)->std::ostream&;

/// Build the targets listed on the command line.
[[nodiscard]] Fn run(data::NinjaMain& ninja_main, Status& status)->Result<void>;

Fn get_ninja_verbosity()->BuildConfig::Verbosity;

// Limit number of rebuilds, to prevent infinite loops.
inline constexpr i32 rebuildLimit = 100;

[[nodiscard]] Fn start(
    const toml::value& poac_manifest, const Mode& mode,
    const resolver::ResolvedDeps& resolved_deps
)
    ->Result<Path>;

} // namespace poac::core::builder::build
