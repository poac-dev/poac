#ifndef POAC_CORE_BUILDER_NINJA_BUILD_HPP_
#define POAC_CORE_BUILDER_NINJA_BUILD_HPP_

// std
#include <ostream>
#include <string>

// external
#include <ninja/state.h> // NOLINT(build/include_order)
#include <toml.hpp>

// internal
#include "poac/core/builder/ninja/data.hpp"
#include "poac/core/resolver/types.hpp" // ResolvedDeps
#include "poac/poac.hpp"

namespace poac::core::builder::ninja::build {

using GeneralError =
    Error<"internal build system has been stopped with an error:\n{}", String>;

enum class Mode {
  debug,
  release,
};

String
to_string(Mode mode);

std::ostream&
operator<<(std::ostream& os, Mode mode);

/// Build the targets listed on the command line.
[[nodiscard]] Result<void>
run(data::NinjaMain& ninja_main, Status& status);

BuildConfig::Verbosity
get_ninja_verbosity();

inline const String progress_status_format =
    format("{:>25} %f/%t: ", "Compiling"_bold_green);

// Limit number of rebuilds, to prevent infinite loops.
inline constexpr i32 rebuildLimit = 100;

[[nodiscard]] Result<Path>
start(
    const toml::value& poac_manifest, const Mode& mode,
    const resolver::ResolvedDeps& resolved_deps
);

} // namespace poac::core::builder::ninja::build

#endif // POAC_CORE_BUILDER_NINJA_BUILD_HPP_
