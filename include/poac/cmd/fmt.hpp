#ifndef POAC_CMD_FMT_HPP_
#define POAC_CMD_FMT_HPP_

// external
#include <structopt/app.hpp>

// internal
#include "poac/poac.hpp"

namespace poac::cmd::fmt {

struct Options : structopt::sub_command {
  // Perform only checks
  Option<bool> check = false;
};

using ClangFormatNotFound = Error<
    "`fmt` command requires `clang-format`; try installing it by:\n"
    "  apt/brew install clang-format">;

inline constexpr StringRef directories[] = {
    "examples", "include", "lib", "src", "tests"};

inline constexpr StringRef extensions[] = {"c",   "c++", "cc",  "cpp",
                                           "cu",  "cuh", "cxx", "h",
                                           "h++", "hh",  "hpp", "hxx"};

inline constexpr StringRef patterns[] = {"./{}/*.{}", "./{}/**/*.{}"};

[[nodiscard]] Result<void>
fmt(StringRef args);

[[nodiscard]] Result<void>
exec(const Options& opts);

} // namespace poac::cmd::fmt

STRUCTOPT(poac::cmd::fmt::Options, check);

#endif // POAC_CMD_FMT_HPP_
